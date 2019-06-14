// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "mouse.h"

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

mouse::mouse( ::platform::system *s ) : ::platform::mouse( s ) {}

////////////////////////////////////////

mouse::~mouse( void ) {}

////////////////////////////////////////

void mouse::start( void ) {}

////////////////////////////////////////

void mouse::cancel( void ) {}

////////////////////////////////////////

void mouse::shutdown( void ) {}

////////////////////////////////////////

waitable::wait mouse::poll_object( void ) { return INVALID_WAIT; }

////////////////////////////////////////

bool mouse::poll_timeout( duration &when, const time_point &curtime )
{
    return false;
}

////////////////////////////////////////

void mouse::emit( const time_point &curtime ) {}

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
