
#include "button.h"

namespace
{

draw::gradient grad
{
	{ 0.0, { 0.9608, 0.9608, 0.9608 } },
	{ 1.0, { 0.7373, 0.7373, 0.7373 } }
};

}

////////////////////////////////////////

namespace cocoa
{

////////////////////////////////////////

button::button( void )
{
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

void button::paint( const std::shared_ptr<draw::canvas> &canvas, const draw::rect &area )
{
	draw::paint p( { 0.4235, 0.4235, 0.4235 } );
	p.set_stroke_width( 1.5 );
	p.set_fill_linear( area.top_left(), area.bottom_left(), grad );

	draw::path rpath;
	rpath.rounded_rect( area.top_left(), area.bottom_right(), 6 );

	canvas->draw_path( rpath, p );
}

////////////////////////////////////////

}

