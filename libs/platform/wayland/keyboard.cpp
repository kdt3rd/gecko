//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include <iostream>
#include "keyboard.h"
#include <map>
#include <base/pointer.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>

////////////////////////////////////////

namespace
{

////////////////////////////////////////

}

////////////////////////////////////////

namespace platform { namespace wayland
{

////////////////////////////////////////

keyboard::keyboard( ::platform::system *s )
	: ::platform::keyboard( s )
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

waitable::wait
keyboard::poll_object( void )
{
	return INVALID_WAIT;
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

} }

