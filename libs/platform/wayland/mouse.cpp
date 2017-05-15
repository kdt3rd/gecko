//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "mouse.h"

namespace platform { namespace wayland
{

////////////////////////////////////////

mouse::mouse( void )
{
}

////////////////////////////////////////

mouse::~mouse( void )
{
}

////////////////////////////////////////

void mouse::start( void )
{
}

////////////////////////////////////////

void mouse::cancel( void )
{
}

////////////////////////////////////////

void mouse::shutdown( void )
{
}

////////////////////////////////////////

intptr_t mouse::poll_object( void )
{
	return intptr_t(-1);
}

////////////////////////////////////////

bool mouse::poll_timeout( duration &when, const time_point &curtime )
{
	return false;
}

////////////////////////////////////////

void mouse::emit( const time_point &curtime )
{
}

////////////////////////////////////////

} }

