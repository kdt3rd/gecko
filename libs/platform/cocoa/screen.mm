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

double screen::refresh_rate( void ) const
{
	// TODO: implement. Look at CVDisplayLink?
	return 30.0;
}


////////////////////////////////////////

rect screen::bounds( bool avail ) const
{
	NSScreen *scr = reinterpret_cast<NSScreen *>( _nsscreen );
	NSRect b = [scr frame];
	auto scale = [scr backingScaleFactor];
	// TODO: show avail or not
	return { 0, 0, static_cast<coord_type>( b.size.width * scale ), static_cast<coord_type>( b.size.height * scale ) };
}

////////////////////////////////////////

base::dsize screen::dpi( void ) const
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


