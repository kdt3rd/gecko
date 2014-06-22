
#include <iostream>
#include "background_color.h"

namespace gui
{

////////////////////////////////////////

background_color::background_color( datum<core::color> &&c, const std::shared_ptr<widget> &w )
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
//	canvas->fill( *this, _color.value() );
	background::paint( canvas );
}

////////////////////////////////////////

}

