
#include <iostream>
#include "button.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

button::button( void )
{
}

////////////////////////////////////////

button::button( std::string l, base::alignment a, const base::color &c, const std::shared_ptr<script::font> &f )
	: _text( std::move( l ) ), _align( a ), _color( c ), _font( f )
{
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
	if ( !_draw )
	{
		base::path path;
		path.rounded_rect( { 0, 0 }, 20, 20, 3 );

		base::paint paint;
		paint.set_fill_color( { 0.27, 0.27, 0.27 } );

		_draw = std::make_shared<draw::stretchable>();
		_draw->create( canvas, path, paint, { 10, 10 } );
	}

	_draw->set( canvas, *this );
	_draw->draw( *canvas );

	base::rect content( *this );
	content.shrink( 6, 6, 3, 3 );
	base::point p = canvas->align_text( _font, _text, content, _align );
	base::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, p, _text, paint );
}

////////////////////////////////////////

void button::compute_minimum( void )
{
	script::font_extents fex = _font->extents();
	script::text_extents tex = _font->extents( _text );

	base::size textsize( tex.x_advance, fex.height );

	base::size full( textsize );
	full.grow( 12, 6 );
	full.ceil();
	full.set_height( std::max( full.h(), 21.0 ) );

	set_minimum( full );
}

////////////////////////////////////////

bool button::mouse_press( const base::point &p, int /*button*/ )
{
	if ( contains( p ) )
	{
		_tracking = true;
		set_pressed( true );
	}
	return _tracking;
}

////////////////////////////////////////

bool button::mouse_release( const base::point &p, int /*button*/ )
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

bool button::mouse_move( const base::point &p )
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

