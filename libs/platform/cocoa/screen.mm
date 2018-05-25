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
	_standard = color::make_standard<color::standard::SRGB>();
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

bool screen::is_managed( void ) const
{
	return true;
}

////////////////////////////////////////

bool screen::is_remote( void ) const
{
	return false;
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

double screen::refresh_rate( void ) const
{
	// TODO: implement. Look at CVDisplayLink?
	return 30.0;
}

////////////////////////////////////////

const color::standard_definition &screen::display_standard( void ) const
{
	return _standard;
}

////////////////////////////////////////

void screen::override_display_standard( const color::standard_definition &s )
{
	_override_standard = true;
	_standard = s;
}

////////////////////////////////////////

} }


