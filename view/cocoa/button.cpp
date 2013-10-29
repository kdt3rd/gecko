
#include "button.h"

namespace
{

draw::gradient grad
{
	{ 0.00, { 1.0000, 1.0000, 1.0000 } },
	{ 0.22, { 1.0000, 1.0000, 1.0000 } },
	{ 0.56, { 0.9255, 0.9255, 0.9255 } },
	{ 0.72, { 0.9255, 0.9255, 0.9255 } },
	{ 0.83, { 0.9294, 0.9294, 0.9294 } },
	{ 0.94, { 0.9373, 0.9373, 0.9373 } },
	{ 1.00, { 0.9490, 0.9490, 0.9490 } },
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
	draw::paint p( { 0.6039, 0.6039, 0.6039 } );
	p.set_stroke_width( 1.0 );
	p.set_fill_linear( area.top_left(), area.bottom_left(), grad );

	draw::path rpath;
	rpath.rounded_rect( area.top_left(), area.bottom_right(), 3 );

	canvas->draw_path( rpath, p );
}

////////////////////////////////////////

}

