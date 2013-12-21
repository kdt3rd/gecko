
#include <iostream>
#include "slider.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

slider::slider( void )
{
	std::unique_ptr<reaction::slider> act( new reaction::slider() );
	act->move_began.callback( [=]( double v ) { this->_pressed = true; this->invalidate(); } );
	act->changed.callback( [=]( double v ) { this->set_value( v ); } );
	act->move_ended.callback( [=]( double v ) { this->_pressed = false; this->invalidate(); } );

	_action = std::move( act );
}

////////////////////////////////////////

slider::~slider( void )
{
}

////////////////////////////////////////

void slider::set_range( double min, double max )
{
	precondition( min < max, "invalid range" );
	_min = min;
	_max = max;
}

////////////////////////////////////////

void slider::set_value( double v )
{
	_value = v;
	invalidate();
}

////////////////////////////////////////

void slider::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::rect r = rectangle();
	reaction::slider *slide = dynamic_cast<reaction::slider*>( _action.get() );
	if ( slide )
		slide->set_handle_size( r.radius() );

	auto style = application::current()->get_style();
	style->slider_groove( canvas, r );
	style->slider_button( canvas, r, _pressed, _value );
}

////////////////////////////////////////

void slider::compute_minimum( void )
{
	set_minimum( 17.0 * 2.0, 17.0 );
}

////////////////////////////////////////

}

