// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "mouse.h"

namespace platform
{
namespace xlib
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

} // namespace xlib
} // namespace platform
