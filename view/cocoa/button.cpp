
#include "button.h"

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

////////////////////////////////////////

namespace cocoa
{

////////////////////////////////////////

button::button( const std::shared_ptr<draw::area> &a )
	: _area( a )
{
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

void button::layout( const std::shared_ptr<draw::canvas> &canvs )
{
	_area->set_minimum( 150, 21 );
}

////////////////////////////////////////

void button::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::paint p;

	if ( _pressed )
	{
		p.set_stroke_color( border2 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( _area->top_left(), _area->bottom_left(), grad2 );
	}
	else
	{
		p.set_stroke_color( border1 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( _area->top_left(), _area->bottom_left(), grad1 );
	}

	draw::path rpath;
	rpath.rounded_rect( _area->top_left(), _area->bottom_right(), 3 );

	canvas->draw_path( rpath, p );
}

////////////////////////////////////////

void button::set_pressed( bool p )
{
	if ( p != _pressed )
	{
		_pressed = p;
		invalidate( _area->rectangle() );
	}
}

////////////////////////////////////////

}

