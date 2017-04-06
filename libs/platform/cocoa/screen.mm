//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <base/contract.h>
#include <string>
#include <stdexcept>

#include <Cocoa/Cocoa.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

screen::screen( void *scr )
{
	_nsscreen = scr;
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

bool screen::is_default( void ) const
{
	return ( _nsscreen == [NSScreen mainScreen] );
}

////////////////////////////////////////

base::size screen::bounds( void ) const
{
	NSScreen *scr = (NSScreen *)_nsscreen;
	NSRect b = [scr frame];
	float scale = [scr backingScaleFactor];
	return { b.size.width * scale, b.size.height * scale };
}

////////////////////////////////////////

base::size screen::dpi( void ) const
{
	NSScreen *scr = (NSScreen *)_nsscreen;
	NSDictionary *desc = [scr deviceDescription];
	NSValue *val = [desc objectForKey:NSDeviceResolution];
	NSSize dpi;
	[val getValue:&dpi];
	return { dpi.width, dpi.height };
}

////////////////////////////////////////

} }


