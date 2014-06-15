
#include <iostream>
#include "button.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

button::button( void )
{
	auto style = application::current_style();
	_color = style->label_color();
	callback_invalidate( _text, _align, _color, _font );
}

////////////////////////////////////////

button::button( datum<std::string> &&l, datum<alignment> &&a, datum<core::color> &&c, shared_datum<draw::font> &&f )
	: _text( std::move( l ) ), _align( std::move( a ) ), _color( std::move( c ) ), _font( std::move( f ) )
{
	if ( _color.value().alpha() < 0.0 )
	{
		auto style = application::current_style();
		_color = style->label_color();
	}
	callback_invalidate( _text, _align, _color, _font );
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
	auto style = application::current_style();
	style->button_frame( canvas, *this, _pressed );

	core::rect content = style->button_content( *this );
	core::point p = canvas->align_text( _font.value(), _text.value(), content, _align.value() );

	core::paint paint;
	paint.set_fill_color( _color.value() );
	canvas->draw_text( _font.value(), p, _text.value(), paint );
}

////////////////////////////////////////

void button::compute_minimum( void )
{
	draw::font_extents fex = _font.value()->extents();
	draw::text_extents tex = _font.value()->extents( _text.value() );

	core::size s( tex.x_advance, fex.height );

	auto style = application::current_style();
	s = style->button_size( s );

	set_minimum( s );
}

////////////////////////////////////////

bool button::mouse_press( const core::point &p, int button )
{
	if ( contains( p ) )
	{
		_tracking = true;
		set_pressed( true );
	}
	return _tracking;
}

////////////////////////////////////////

bool button::mouse_release( const core::point &p, int button )
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

bool button::mouse_move( const core::point &p )
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

