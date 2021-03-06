// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "background_color.h"

#include <iostream>

namespace gui
{
////////////////////////////////////////

background_color::background_color(
    const color &c, const std::shared_ptr<widget> &w )
    : background( w ), _color( std::move( c ) )
{}

////////////////////////////////////////

background_color::~background_color( void ) {}

////////////////////////////////////////

void background_color::paint( const std::shared_ptr<draw::canvas> &canvas )
{
    //	canvas->fill( *this, _color );
    background::paint( canvas );
}

////////////////////////////////////////

} // namespace gui
