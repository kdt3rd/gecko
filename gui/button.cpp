
#include <iostream>
#include "button.h"
#include "application.h"
#include "style.h"
#include <reaction/button.h>


namespace gui
{

////////////////////////////////////////

button::button( const std::string &t )
{
	_text = t;
	_font = application::current()->get_style()->default_font();

	std::unique_ptr<reaction::button> act( new reaction::button );
	act->pressed.callback( [=]( bool p ) { this->set_pressed( p ); } );

	_action = std::move( act );
}

////////////////////////////////////////

button::button( void )
{
	_font = application::current()->get_style()->default_font();
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

void button::set_pressed( bool p )
{
	if ( p != _pressed )
	{
		_pressed = p;
		invalidate( rectangle() );
	}
}

////////////////////////////////////////

void button::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	auto style = application::current()->get_style();
	style->button_frame( canvas, rectangle(), _pressed );

	draw::point p = canvas->align_text( _font, _text, rectangle(), _align );

	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, p, _text, paint );
}

////////////////////////////////////////

void button::layout( void )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );
	set_minimum( tex.width - tex.x_bearing, std::max( 21.0, fex.height ) );
}

////////////////////////////////////////

}

