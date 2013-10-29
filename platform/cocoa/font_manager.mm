
#import <AppKit/NSFontManager.h>
#import <Foundation/NSString.h>
#import <ApplicationServices/ApplicationServices.h>
#import <CoreText/CTFont.h>

#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <draw/cairo/font.h>
#include <cairo/cairo-quartz.h>

namespace cocoa
{

////////////////////////////////////////

font_manager::font_manager( void )
{
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

std::set<std::string> font_manager::get_foundries( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;

	NSFontManager *fmgr = [NSFontManager sharedFontManager];
	for ( id name in [fmgr availableFontFamilies] )
		ret.insert( [(NSString*)name UTF8String] );
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( void )
{
	std::set<std::string> ret;
	ret.insert( "Italic" );
	ret.insert( "Bold" );
	ret.insert( "Narrow" );
	ret.insert( "Expanded" );
	ret.insert( "Condensed" );
	ret.insert( "Small Caps" );
	ret.insert( "Compressed" );
	ret.insert( "Fixed Pitch" );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<draw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::shared_ptr<cairo::font> ret;

	NSFontManager *fmgr = [NSFontManager sharedFontManager];
	NSString *fam = [[NSString alloc] initWithUTF8String:family.c_str()];
	NSFontTraitMask mask = 0;
	NSFont *nsfont = [fmgr fontWithFamily:fam traits:mask weight:5 size:pixsize];
	if ( nsfont )
	{
		CGFontRef font = CTFontCopyGraphicsFont( (CTFontRef)nsfont, NULL );
		ret = std::make_shared<cairo::font>( cairo_quartz_font_face_create_for_cgfont( font ), family, style, pixsize );
	}
	
	return ret;
}

////////////////////////////////////////

}

