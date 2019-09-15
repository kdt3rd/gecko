// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "keyboard.h"

////////////////////////////////////////

namespace platform
{
namespace xlib
{
////////////////////////////////////////

keyboard::keyboard( ::platform::system *s ) : ::platform::keyboard( s ) {}

////////////////////////////////////////

keyboard::~keyboard( void ) {}

////////////////////////////////////////

void keyboard::start( void ) {}

////////////////////////////////////////

void keyboard::cancel( void ) {}

////////////////////////////////////////

void keyboard::shutdown( void ) {}

////////////////////////////////////////

waitable::wait keyboard::poll_object( void ) { return INVALID_WAIT; }

////////////////////////////////////////

bool keyboard::poll_timeout( duration &, const time_point & ) { return false; }

////////////////////////////////////////

void keyboard::emit( const time_point &curtime ) {}

////////////////////////////////////////

} // namespace xlib
} // namespace platform
