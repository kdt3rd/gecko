
#include <iostream>
#include "flat.h"

////////////////////////////////////////

namespace view
{

////////////////////////////////////////

flat::flat( const std::shared_ptr<layout::area> &a )
	: _area( a )
{
	_area->set_minimum( 100, 21 );
}

////////////////////////////////////////

flat::~flat( void )
{
}

////////////////////////////////////////

void flat::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->fill( _area->rectangle(), paint );
}

////////////////////////////////////////

}

