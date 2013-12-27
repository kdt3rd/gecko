
#include <iostream>
#include "button.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

button::button( const std::string &t )
{
	_text = t;
	_font = application::current()->get_style()->default_font();
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
		invalidate();
	}
}

////////////////////////////////////////

void button::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	auto style = application::current()->get_style();
	style->button_frame( canvas, *this, _pressed );

	draw::point p = canvas->align_text( _font, _text, *this, _align );

	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, p, _text, paint );
}

////////////////////////////////////////

void button::compute_minimum( void )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );
	set_minimum( tex.x_advance + 12, std::max( 21.0, fex.height ) );
}

////////////////////////////////////////

bool button::mouse_press( const draw::point &p, int button )
{
	if ( contains( p ) )
	{
		_tracking = true;
		set_pressed( true );
	}
	return _tracking;
}

////////////////////////////////////////

bool button::mouse_release( const draw::point &p, int button )
{
	if ( _tracking )
	{
		_tracking = false;
		set_pressed( false );
		if ( contains( p ) )
			when_activated();
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_move( const draw::point &p )
{
	if ( _tracking )
	{
		set_pressed( contains( p ) );
		return true;
	}
	return false;
}

////////////////////////////////////////

}

