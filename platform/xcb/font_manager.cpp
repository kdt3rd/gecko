
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace xcb
{

////////////////////////////////////////

font_manager::font_manager( void )
{
	if ( FcInit() != FcTrue )
		throw std::runtime_error( "error intializing fontconfig" );

	set_manager_name( "fontconfig" );
	std::stringstream version;
	int v = FcGetVersion();
	version << v/10000 << '.' << (v/100)%100 << '.' << v%100;
	set_manager_version( version.str() );

	_config = FcConfigGetCurrent();
	FcConfigSetRescanInterval( _config, 0 );
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
	FcFini();
}

////////////////////////////////////////

std::set<std::string> font_manager::get_foundries( void )
{
	std::set<std::string> ret;

	FcPattern *pat = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild( FC_FOUNDRY, nullptr );
	FcFontSet *fs = FcFontList( _config, pat, os );
	if ( fs )
	{
		FcChar8 *name = nullptr;
		for ( int i = 0; i < fs->nfont; ++i )
		{
			FcPattern *font = fs->fonts[i];
			if ( FcPatternGetString( font, FC_FOUNDRY, 0, &name ) == FcResultMatch )
				ret.emplace( reinterpret_cast<const char*>( name ) );
		}
		FcFontSetDestroy( fs );
	}
	FcObjectSetDestroy( os );
	FcPatternDestroy( pat );

	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;

	FcPattern *pat = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild( FC_FAMILY, nullptr );
	FcFontSet *fs = FcFontList( _config, pat, os );
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

std::set<std::string> font_manager::get_styles( void )
{
	std::set<std::string> ret;

	FcPattern *pat = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild( FC_STYLE, nullptr );
	FcFontSet *fs = FcFontList( _config, pat, os );
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

}

