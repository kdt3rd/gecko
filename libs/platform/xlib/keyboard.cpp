//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "keyboard.h"

////////////////////////////////////////

namespace platform { namespace xlib
{

////////////////////////////////////////

keyboard::keyboard( void )
{
}

////////////////////////////////////////

keyboard::~keyboard( void )
{
}

////////////////////////////////////////

void keyboard::start( void )
{
}

////////////////////////////////////////

void keyboard::cancel( void )
{
}

////////////////////////////////////////

void keyboard::shutdown( void )
{
}

////////////////////////////////////////

intptr_t keyboard::poll_object( void )
{
	return intptr_t(-1);
}

////////////////////////////////////////

bool keyboard::poll_timeout( duration &, const time_point & )
{
	return false;
}

////////////////////////////////////////

void keyboard::emit( const time_point &curtime )
{
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform

