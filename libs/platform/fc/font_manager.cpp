
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <tuple>
#include <map>
#include <vector>
#include <base/scope_guard.h>

#include <fontconfig/fontconfig.h>

#include "glyph.h"
#include "font.h"

namespace platform { namespace fc
{

struct font_manager::pimpl
{
	FcConfig *config = nullptr;
	FT_Library ftlib;
};

////////////////////////////////////////

font_manager::font_manager( void )
	: _impl( new pimpl )
{
	if ( FcInit() != FcTrue )
		throw std::runtime_error( "error intializing fontconfig" );

	set_manager_name( "fontconfig" );
	std::stringstream version;
	int v = FcGetVersion();
	version << v/10000 << '.' << (v/100)%100 << '.' << v%100;
	set_manager_version( version.str() );

	_impl->config = FcConfigGetCurrent();
	FcConfigSetRescanInterval( _impl->config, 0 );

	if ( FT_Init_FreeType( &(_impl->ftlib) ) != 0 )
		throw std::runtime_error( "freetype initialization failed" );
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
	FT_Done_FreeType( _impl->ftlib );

	delete _impl;
	FcFini();
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;

	FcPattern *pat = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild( FC_FAMILY, nullptr );
	FcFontSet *fs = FcFontList( _impl->config, pat, os );
	if ( fs )
	{
		FcChar8 *name = nullptr;
		for ( int i = 0; i < fs->nfont; ++i )
		{
			FcPattern *font = fs->fonts[i];
			if ( FcPatternGetString( font, FC_FAMILY, 0, &name ) == FcResultMatch )
				ret.emplace( reinterpret_cast<const char*>( name ) );
		}
		FcFontSetDestroy( fs );
	}
	FcObjectSetDestroy( os );
	FcPatternDestroy( pat );

	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	std::set<std::string> ret;

	FcPattern *pat = FcPatternBuild( nullptr, FC_FAMILY, FcTypeString, family.c_str(), nullptr );
	FcObjectSet *os = FcObjectSetBuild( FC_STYLE, nullptr );
	FcFontSet *fs = FcFontList( _impl->config, pat, os );
	if ( fs )
	{
		FcChar8 *name = nullptr;
		for ( int i = 0; i < fs->nfont; ++i )
		{
			FcPattern *font = fs->fonts[i];
			if ( FcPatternGetString( font, FC_STYLE, 0, &name ) == FcResultMatch )
				ret.emplace( reinterpret_cast<const char*>( name ) );
		}
		FcFontSetDestroy( fs );
	}
	FcObjectSetDestroy( os );
	FcPatternDestroy( pat );

	return ret;
}

////////////////////////////////////////

std::shared_ptr<draw::font>
font_manager::get_font( const std::string &family, const std::string &style,
						double pixsize )
{
	FcPattern *pat = FcPatternBuild( nullptr,
		FC_FAMILY, FcTypeString, family.c_str(),
		FC_STYLE, FcTypeString, style.c_str(),
		FC_PIXEL_SIZE, FcTypeDouble, pixsize,
		nullptr );

	if ( ! pat )
		throw std::runtime_error( "Unable to build font pattern" );

	FcResult result;
	FcPattern *matched = FcFontMatch( _impl->config, pat, &result );
	FcPatternDestroy( pat );
	on_scope_exit { if ( matched ) FcPatternDestroy( matched ); };

	std::shared_ptr<draw::font> ret;
	if ( matched && result == FcResultMatch )
	{
		FcChar8 *filename = nullptr;
		int fontid = 0;
		if ( FcPatternGetString( matched, FC_FILE, 0, &filename ) == FcResultMatch &&
			 FcPatternGetInteger( matched, FC_INDEX, 0, &fontid ) == FcResultMatch )
		{
			FT_Face ftface;
			auto error = FT_New_Face( _impl->ftlib, reinterpret_cast<const char*>( filename ), fontid, &ftface );
			if ( error )
				throw std::runtime_error( fc::font::errorstr( error ) );

			try
			{
				ret = std::make_shared<fc::font>( ftface, family, style, pixsize );
			}
			catch ( ... )
			{
				FT_Done_Face( ftface );
			}
		}
		else
			throw std::runtime_error( "Unable to extract font information" );
	}
	else
		throw std::runtime_error( "No matching font found" );

	return ret;
}

////////////////////////////////////////

} }

