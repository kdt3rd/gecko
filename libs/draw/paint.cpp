// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT

#include "paint.h"

namespace draw
{

////////////////////////////////////////

paint::paint( void )
{
}

////////////////////////////////////////

paint::paint( const color &c, dim w )
	: _stroke_color( c ), _stroke_width( w )
{
}

////////////////////////////////////////

paint::~paint( void )
{
}

////////////////////////////////////////

void paint::clear_fill( void )
{
	switch ( _fill_type )
	{
		case NONE:
		case COLOR:
			break;

		case LINEAR:
			_fill_linear.~linear();
			break;

		case RADIAL:
			_fill_radial.~radial();
			break;

		case CONICAL:
			_fill_conical.~conical();
			break;

		case BOX:
			_fill_box.~box();
			break;
	}
	_fill_type = NONE;
}

////////////////////////////////////////

}
