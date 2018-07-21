//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font_manager.h"
#include <script/freetype2/font.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <tuple>
#include <map>
#include <vector>
#include <base/scope_guard.h>
#include <base/language.h>
#include <base/contract.h>
#include <windows.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{

//struct font_info_cache
//{
//	struct family_info
//	{
//		ENUMLOGFONTEXDV logical_font_info;
//		NEWTEXTMETRICEX text_metrics;
//	};
//	struct family_data
//	{
//		std::map<uint8_t, std::vector<family_info>> charset_info;
//		std::set<std::string> styles;
//	};
//
//	std::map<std::string, family_data> avail_fonts;
//};

int CALLBACK queryFamilies( const LOGFONT *info,
							const TEXTMETRIC * /*metrics*/,
							DWORD /*fonttype*/,
							LPARAM userdata )
{
	// do we care about true type or anything for real?
	std::set<std::string> *famlist = reinterpret_cast<std::set<std::string> *>( userdata );
	famlist->insert( std::string( reinterpret_cast<const char *>( info->lfFaceName ) ) );
	return 1;
}

int CALLBACK queryStyles( const LOGFONT *info,
						  const TEXTMETRIC * /*metrics*/,
						  DWORD /*fonttype*/,
						  LPARAM userdata )
{
	// do we care about true type or anything for real?
	std::set<std::string> *styleList = reinterpret_cast<std::set<std::string> *>( userdata );
	const ENUMLOGFONTEXDV *exinfo = reinterpret_cast<const ENUMLOGFONTEXDV *>( info );
	styleList->insert( std::string( reinterpret_cast<const char *>( exinfo->elfEnumLogfontEx.elfStyle ) ) );
	return 1;
}

}

namespace script { namespace mswin
{

struct font_manager::pimpl
{
	FT_Library ftlib;
};

////////////////////////////////////////

font_manager::font_manager( void )
	: _impl( new pimpl )
{
	if ( FT_Init_FreeType( &(_impl->ftlib) ) != 0 )
		throw std::runtime_error( "freetype initialization failed" );
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
	FT_Done_FreeType( _impl->ftlib );

	delete _impl;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	HDC dc = CreateCompatibleDC( NULL );
	if ( dc == NULL )
		throw_lasterror( "unable to create device for text query" );
	on_scope_exit{ DeleteDC( dc ); };

	// This does not seem to return any font 'aliases' (i.e. Arial under wine)...

	LOGFONT fquery;
	fquery.lfCharSet = DEFAULT_CHARSET;
	fquery.lfFaceName[0] = '\0';
	fquery.lfPitchAndFamily = 0;
	
	std::set<std::string> ret;

	EnumFontFamiliesEx( dc, &fquery, queryFamilies, reinterpret_cast<LPARAM>( &ret ), 0 );

	// TODO: should we cache this? if we do, we have to monitor when
	// things change... let's assume it won't be called that often.
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	HDC dc = CreateCompatibleDC( NULL );
	if ( dc == NULL )
		throw_lasterror( "unable to create device for text query" );
	on_scope_exit{ DeleteDC( dc ); };

	LOGFONT fquery;
	fquery.lfCharSet = DEFAULT_CHARSET;
	fquery.lfFaceName[0] = '\0';
	if ( ! family.empty() )
	{
		size_t last = std::min( size_t(LF_FACESIZE - 1), family.size() );
		strncpy( fquery.lfFaceName, family.c_str(), last );
		fquery.lfFaceName[last] = '\0';
	}
	fquery.lfPitchAndFamily = 0;
	
	std::set<std::string> ret;

	EnumFontFamiliesEx( dc, &fquery, queryStyles, reinterpret_cast<LPARAM>( &ret ), 0 );

	return ret;
}

////////////////////////////////////////

std::shared_ptr<script::font>
font_manager::get_font( const std::string &family, const std::string &style, points pts )
{
	std::string lang = base::locale::language();

	std::shared_ptr<script::font> ret;

	HDC dc = CreateCompatibleDC( NULL );
	if ( dc == NULL )
		throw_lasterror( "unable to create device for text query" );
	on_scope_exit{ DeleteDC( dc ); };

	LOGFONT fquery = {0};
	fquery.lfCharSet = DEFAULT_CHARSET;
	fquery.lfHeight = - MulDiv( static_cast<int>( pts.count() ), GetDeviceCaps( dc, LOGPIXELSY ), 72 );
	fquery.lfWidth = 0;
	fquery.lfWeight = FW_DONTCARE;

	// TODO: is there a better way to do this???
	if ( ! style.empty() )
	{
		static const std::map<std::string, LONG> styleToWeight
		{
			{ "Extra Light", FW_EXTRALIGHT },
			{ "Ultra Light", FW_ULTRALIGHT },
			{ "Thin", FW_THIN },
			{ "Light", FW_LIGHT },
			{ "Normal", FW_NORMAL },
			{ "Regular", FW_REGULAR },
			{ "Medium", FW_MEDIUM },
			{ "Semi Bold", FW_SEMIBOLD },
			{ "Demi Bold", FW_DEMIBOLD },
			{ "Bold", FW_BOLD },
			{ "Extra Bold", FW_EXTRABOLD },
			{ "Ultra Bold", FW_ULTRABOLD },
			{ "Heavy", FW_HEAVY },
			{ "Black", FW_BLACK }
		};

		auto stw = styleToWeight.find( style );
		if ( stw != styleToWeight.end() )
			fquery.lfWeight = stw->second;
	}

	if ( ! family.empty() )
	{
		size_t last = std::min( size_t(LF_FACESIZE - 1), family.size() );
		strncpy( fquery.lfFaceName, family.c_str(), last );
		fquery.lfFaceName[last] = '\0';
	}

	HFONT fnt = CreateFontIndirect( &fquery );
	if ( fnt == NULL )
		throw_lasterror( "Unable to create font with family '{0}', style '{1}'", family, style );

	SelectObject( dc, fnt );

	DWORD ttfsz = GetFontData( dc, 0, 0, NULL, 0 );
	if ( ttfsz == GDI_ERROR )
		throw_lasterror( "unable to retrieve font data size" );

	std::shared_ptr<uint8_t []> ttfData( new uint8_t[ttfsz + 1] );
	ttfsz = GetFontData( dc, 0, 0, ttfData.get(), ttfsz );
	if ( ttfsz == GDI_ERROR )
		throw_lasterror( "unable to retrieve font data" );

	FT_Face ftface;
	auto error = FT_New_Memory_Face( _impl->ftlib, reinterpret_cast<const FT_Byte *>( ttfData.get() ), static_cast<FT_Long>( ttfsz ), 0, &ftface );
	if ( error )
		throw std::runtime_error( script::freetype2::font::errorstr( error ) );

	try
	{
		ret = std::make_shared<script::freetype2::font>( ftface, family, style, pts, ttfData );
		ret->load_dpi( _dpi_h, _dpi_v );
		ret->max_glyph_store( _max_glyph_w, _max_glyph_h );

		ret->init_font();
	}
	catch ( ... )
	{
		FT_Done_Face( ftface );
		throw;
	}

	return ret;
}

////////////////////////////////////////

} }

