
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
		ret.insert( [(NSString*)name UTF8String] );
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
		NSArray *fontdesc = (NSArray *)fd;
		NSFontTraitMask traits = [[fontdesc objectAtIndex:3] intValue];

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

std::shared_ptr<script::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
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
		
	NSFont *nsfont = [fmgr fontWithFamily:fam traits:mask weight:5 size:pixsize];
	if ( nsfont )
	{
		ret = std::make_shared<script::cocoa::font>( (void *)nsfont, family, style, pixsize );
	}
	else
		throw std::runtime_error( "font not found" );
	
	return ret;
}

////////////////////////////////////////

} }
