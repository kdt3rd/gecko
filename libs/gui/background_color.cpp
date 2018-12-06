//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <iostream>
#include "background_color.h"

namespace gui
{

////////////////////////////////////////

background_color::background_color( const color &c, const std::shared_ptr<widget> &w )
	: background( w ), _color( std::move( c ) )
{
}

////////////////////////////////////////

background_color::~background_color( void )
{
}

////////////////////////////////////////

void background_color::paint( const std::shared_ptr<draw::canvas> &canvas )
{
//	canvas->fill( *this, _color );
	background::paint( canvas );
}

////////////////////////////////////////

}

