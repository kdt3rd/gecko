// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "screen.h"

#include <core/contract.h>
#include <stdexcept>
#include <string>
#include <xcb/xcb.h>

namespace platform
{
namespace xcb
{
////////////////////////////////////////

screen::screen( xcb_screen_t *scr ) : _screen( scr )
{
    precondition( _screen, "null screen" );
}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

draw::size screen::bounds( void )
{
    precondition( _screen, "null screen" );
    return { double( _screen->width_in_pixels ),
             double( _screen->height_in_pixels ) };
}

////////////////////////////////////////

} // namespace xcb
} // namespace platform
