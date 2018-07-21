
#import <AppKit/NSFontManager.h>
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <ApplicationServices/ApplicationServices.h>
#import <CoreText/CTFont.h>

#include "font_manager.h"
#include "font.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <map>

namespace
{
	const std::map<NSFontTraitMask,std::string> fonttraits =
	{
		{ NSItalicFontMask, "Italic" },
		{ NSBoldFontMask, "Bold" },
		{ NSNarrowFontMask, "Narrow" },
		{ NSExpandedFontMask, "Expanded" },
		{ NSSmallCapsFontMask, "SmallCaps" },
		{ NSCompressedFontMask, "Compressed" },
		{ NSFixedPitchFontMask, "Fixed" },
	};
}

////////////////////////////////////////

namespace script { namespace cocoa
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

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;

	NSFontManager *fmgr = [NSFontManager sharedFontManager];
	for ( id name in [fmgr availableFontFamilies] )
		ret.insert( [static_cast<NSString*>(name) UTF8String] );
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	std::set<std::string> ret;

	NSFontManager *fmgr = [NSFontManager sharedFontManager];

	NSString *fam = [NSString stringWithUTF8String:family.c_str()];

	for ( id fd in [fmgr availableMembersOfFontFamily:fam] )
	{
		NSArray *fontdesc = static_cast<NSArray *>( fd );
		NSFontTraitMask traits = static_cast<NSFontTraitMask>( [[fontdesc objectAtIndex:3] intValue] );

		std::string style;
		for ( auto t: fonttraits )
		{
			if ( ( traits & t.first ) != 0 )
			{
				if ( !style.empty() )
					style += "/";
				style += t.second;
			}
		}

		if ( style.empty() )
			style = "Regular";
		ret.insert( style );
	}
	return ret;
}

////////////////////////////////////////

std::shared_ptr<script::font> font_manager::get_font( const std::string &family, const std::string &style, points pts )
{
	std::shared_ptr<script::font> ret;

	NSFontManager *fmgr = [NSFontManager sharedFontManager];
	NSString *fam = [[NSString alloc] initWithUTF8String:family.c_str()];
	NSFontTraitMask mask = 0;
	if ( style.find( "Italic" ) != std::string::npos )
		mask |= NSItalicFontMask;
	else
		mask |= NSUnitalicFontMask;
	if ( style.find( "Bold" ) != std::string::npos )
		mask |= NSBoldFontMask;
	else
		mask |= NSUnboldFontMask;
	if ( style.find( "Narrow" ) != std::string::npos )
		mask |= NSNarrowFontMask;
	if ( style.find( "Expanded" ) != std::string::npos )
		mask |= NSExpandedFontMask;
	if ( style.find( "Small Caps" ) != std::string::npos )
		mask |= NSSmallCapsFontMask;
	if ( style.find( "Compressed" ) != std::string::npos )
		mask |= NSCompressedFontMask;
	if ( style.find( "Fixed Pitch" ) != std::string::npos )
		mask |= NSFixedPitchFontMask;

	NSFont *nsfont = [fmgr fontWithFamily:fam traits:mask weight:5 size:static_cast<double>( pts.count() )];
	if ( nsfont )
	{
		auto cret = std::make_shared<script::cocoa::font>( static_cast<void *>(nsfont), family, style, pts );
		cret->load_dpi( _dpi_h, _dpi_v );
		cret->max_glyph_store( _max_glyph_w, _max_glyph_h );
        cret->init_font();
        ret = cret;
	}
	else
		throw std::runtime_error( "font not found" );

	return ret;
}

////////////////////////////////////////

} }
