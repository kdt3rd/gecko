
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace fc
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

std::shared_ptr<draw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::cout << "Getting font: " << family << ' ' << style << ' ' << pixsize << std::endl;
	FcPattern *pat = FcPatternBuild( nullptr,
		FC_FAMILY, FcTypeString, family.c_str(),
		FC_STYLE, FcTypeString, style.c_str(),
		FC_PIXEL_SIZE, FcTypeDouble, pixsize,
		nullptr );

	std::cout << "Pattern: " << pat << std::endl;

	FcResult result;
	FcPattern *matched = FcFontMatch( _impl->config, pat, &result );

	std::cout << "Matched: " << matched << std::endl;

	std::shared_ptr<draw::font> ret;
	if ( matched && result == FcResultMatch )
	{
		FcChar8 *filename = nullptr;
		int fontid = 0;
		if ( FcPatternGetString( matched, FC_FILE, 0, &filename ) == FcResultMatch &&
			 FcPatternGetInteger( matched, FC_INDEX, 0, &fontid ) == FcResultMatch )
		{
			std::cout << "Loading: " << filename << ' ' << fontid << std::endl;
			FT_Face ftface;
			auto error = FT_New_Face( _impl->ftlib, reinterpret_cast<const char*>( filename ), fontid, &ftface );
			std::cout << "Loaded: " << ftface << std::endl;
			if ( error )
				throw std::runtime_error( "freetype error" );
//			ret = std::make_shared<cairo::font>( cairo_ft_font_face_create_for_ft_face( ftface, 0 ), family, style, pixsize );
		}
	}
	FcPatternDestroy( pat );
	FcPatternDestroy( matched );

	return ret;
}

////////////////////////////////////////

}

