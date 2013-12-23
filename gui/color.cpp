
#include <iostream>
#include "color.h"

namespace gui
{

////////////////////////////////////////

color::color( void )
{
}

////////////////////////////////////////

color::color( const draw::color &c, double minw, double minh )
	: _color( c )
{
	set_minimum( minw, minh );
}

////////////////////////////////////////

color::~color( void )
{
}

////////////////////////////////////////

void color::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	canvas->fill( *this, _color );
}

////////////////////////////////////////

}

