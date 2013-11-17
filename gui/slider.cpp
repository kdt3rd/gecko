
#include <iostream>
#include "slider.h"
#include "application.h"
#include "style.h"
#include <reaction/button.h>


namespace gui
{

////////////////////////////////////////

slider::slider( void )
{
}

////////////////////////////////////////

slider::~slider( void )
{
}

////////////////////////////////////////

void slider::set_pressed( bool p )
{
	if ( p != _pressed )
	{
		_pressed = p;
		invalidate( rectangle() );
	}
}

////////////////////////////////////////

void slider::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	auto style = application::current()->get_style();
	style->slider_groove( canvas, rectangle() );
	style->slider_button( canvas, rectangle(), _pressed );
}

////////////////////////////////////////

void slider::layout( void )
{
	set_minimum( 17.0 * 2.0, 17.0 );
}

////////////////////////////////////////

}

