
#include <iostream>
#include "button.h"
#include "application.h"
#include "style.h"
#include <reaction/button.h>

namespace
{

draw::color border1 { 0.6039, 0.6039, 0.6039 };
draw::gradient grad1
{
	{ 0.00, { 1.0000, 1.0000, 1.0000 } },
	{ 0.22, { 1.0000, 1.0000, 1.0000 } },
	{ 0.56, { 0.9255, 0.9255, 0.9255 } },
	{ 0.72, { 0.9255, 0.9255, 0.9255 } },
	{ 0.83, { 0.9294, 0.9294, 0.9294 } },
	{ 0.94, { 0.9373, 0.9373, 0.9373 } },
	{ 1.00, { 0.9490, 0.9490, 0.9490 } },
};

draw::color border2 { 0.3059, 0.2863, 0.4314 };
draw::gradient grad2
{
	{ 0.00, { 0.7804, 0.8039, 0.8275 } },
	{ 0.06, { 0.6784, 0.7059, 0.7608 } },
	{ 0.50, { 0.5176, 0.5647, 0.6510 } },
	{ 0.56, { 0.4549, 0.5098, 0.6157 } },
	{ 1.00, { 0.7451, 0.7843, 0.8157 } },
};

}


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
	draw::paint p;

	if ( _pressed )
	{
		p.set_stroke_color( border2 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( top_left(), bottom_left(), grad2 );
	}
	else
	{
		p.set_stroke_color( border1 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( top_left(), bottom_left(), grad1 );
	}

	draw::path rpath;
	rpath.rounded_rect( top_left(), bottom_right(), 3 );

	canvas->draw_path( rpath, p );
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

