// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "screen.h"

#include <base/contract.h>

namespace platform
{
namespace wayland
{
////////////////////////////////////////

screen::screen( void )
    : _standard( color::make_standard( color::standard::SRGB ) )
{}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

bool screen::is_default( void ) const { return true; }

////////////////////////////////////////

bool screen::is_managed( void ) const { return true; }

////////////////////////////////////////

bool screen::is_remote( void ) const { return false; }

////////////////////////////////////////

double screen::refresh_rate( void ) const { return 30.0; }

////////////////////////////////////////

rect screen::bounds( bool active ) const
{
    return rect(
        coord_type( 0 ),
        coord_type( 0 ),
        coord_type( 1024 ),
        coord_type( 720 ) );
}

////////////////////////////////////////

dots_per_unit screen::dpi( void ) const
{
    return { dots_per_unit::coord_type( 95.0 ),
             dots_per_unit::coord_type( 95.0 ) };
}

////////////////////////////////////////

dots_per_unit screen::dpmm( void ) const
{
    return { dots_per_unit::coord_type( 3.75 ),
             dots_per_unit::coord_type( 3.75 ) };
}

////////////////////////////////////////

const color::standard_definition &screen::display_standard( void ) const
{
    return _standard;
}

////////////////////////////////////////

void screen::override_display_standard( const color::standard_definition &s )
{
    _standard = s;
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform
