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

struct font_info_cache
{
	struct family_info
	{
		ENUMLOGFONTEXDV logical_font_info;
		NEWTEXTMETRICEX text_metrics;
	};
	struct family_data
	{
		std::map<uint8_t, std::vector<family_info>> charset_info;
		std::set<std::string> styles;
	};

	std::map<std::string, family_data> avail_fonts;
};

int CALLBACK fontlistCallback( const LOGFONT *info,
							   const TEXTMETRIC *metrics,
							   DWORD fonttype,
							   LPARAM userdata )
{
	if ( ( fonttype & TRUETYPE_FONTTYPE ) != 0 )
	{
		const ENUMLOGFONTEXDV *exinfo = reinterpret_cast<const ENUMLOGFONTEXDV *>( info );
		const NEWTEXTMETRICEX *ntmetrics = reinterpret_cast<const NEWTEXTMETRICEX *>( metrics );
		font_info_cache *cache = reinterpret_cast<font_info_cache *>( userdata );

		auto &cacheinfo = cache->avail_fonts[info->lfFaceName];
		auto &cset = cacheinfo.charset_info[info->lfCharSet];
		cset.emplace_back();
		cset.back().logical_font_info = *exinfo;
		cset.back().text_metrics = *ntmetrics;
		cacheinfo.styles.insert( std::string( reinterpret_cast<const char *>( exinfo->elfEnumLogfontEx.elfStyle ) ) );
	}
	return 1;
}

}

namespace script { namespace mswin
{

struct font_manager::pimpl
{
	font_info_cache font_cache;
	FT_Library ftlib;
};

////////////////////////////////////////

font_manager::font_manager( void )
	: _impl( new pimpl )
{
	if ( FT_Init_FreeType( &(_impl->ftlib) ) != 0 )
		throw std::runtime_error( "freetype initialization failed" );

	HDC dc = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );
	if ( dc == NULL )
		throw_lasterror( "unable to create device for text query" );
	on_scope_exit{ DeleteDC( dc ); };

	LOGFONT fquery;
	fquery.lfCharSet = DEFAULT_CHARSET;
	fquery.lfFaceName[0] = '\0';
	fquery.lfPitchAndFamily = 0;

	EnumFontFamiliesEx( dc, &fquery, &fontlistCallback, reinterpret_cast<LPARAM>( &(_impl->font_cache) ), 0 );

	std::cout << "Available font families:\n";
	for ( auto &fams: _impl->font_cache.avail_fonts )
		std::cout << "  " << fams.first << '\n';
	std::cout << std::endl;
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
	std::set<std::string> ret;

	for ( const auto &f: _impl->font_cache.avail_fonts )
		ret.insert( f.first );

	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	auto f = _impl->font_cache.avail_fonts.find( family );
	if ( f != _impl->font_cache.avail_fonts.end() )
		return f->second.styles;

	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::shared_ptr<script::font>
font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::string lang = base::locale::language();

	std::shared_ptr<script::font> ret;

	// TODO: make this logic more fuzzy - there is font
	// matching logic we could use...
	auto f = _impl->font_cache.avail_fonts.find( family );
	if ( f != _impl->font_cache.avail_fonts.end() )
	{
//		HDC dc = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );
//		if ( dc == NULL )
//			throw_lasterror( "unable to create device for text query" );
//		on_scope_exit{ DeleteDC( dc ); };
		std::cout << "Need to determine charset from language in order to choose the appropriate font..." << std::endl;

		auto fci = f->second.charset_info.find( uint8_t(0) );
		if ( fci != f->second.charset_info.end() )
		{
			auto &faminfo = fci->second;
			HFONT fnt = NULL;
			for ( const auto &lf: faminfo )
			{
				if ( style == reinterpret_cast<const char *>( lf.logical_font_info.elfEnumLogfontEx.elfStyle ) )
				{
					fnt = CreateFontIndirectEx( &(lf.logical_font_info) );
					break;
				}
			}

			if ( fnt == NULL )
				throw_runtime( "Found font family '{0}' and charset, but unable to find style '{1}'", family, style );

			HDC dc = CreateDC( TEXT("DISPLAY"), NULL, NULL, NULL );
			if ( dc == NULL )
				throw_lasterror( "unable to create device for text query" );
			on_scope_exit{ DeleteDC( dc ); };

			SelectObject( dc, fnt );

			DWORD ttfsz = GetFontData( dc, 0, 0, NULL, 0 );
			if ( ttfsz == GDI_ERROR )
				throw_lasterror( "unable to retrieve font data size" );

			std::cout << "font data is: " << ttfsz << " bytes" << std::endl;
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
				ret = std::make_shared<script::freetype2::font>( ftface, family, style, pixsize, ttfData );
			}
			catch ( ... )
			{
				FT_Done_Face( ftface );
			}
			
		}
		else
			throw_runtime( "Found font family '{0}', but no default char set", family );
	}
	else
		throw_runtime( "Unable to find font family '{0}'", family );

	return ret;
}

////////////////////////////////////////

} }

