
#include "canvas.h"

namespace draw
{

////////////////////////////////////////

canvas::canvas( void )
{
}

////////////////////////////////////////

canvas::~canvas( void )
{
}

////////////////////////////////////////

/*
void canvas::fill( const core::color &c )
{
	paint p( c );
	fill( p );
}

////////////////////////////////////////

core::point canvas::align_text( const std::shared_ptr<font> &font, const std::string &text, const core::rect &rect, alignment a )
{
	font_extents fex = font->extents();
	text_extents tex = font->extents( text );

	double y = 0.0, x = 0.0;

	switch ( a )
	{
		case alignment::CENTER:
		case alignment::LEFT:
		case alignment::RIGHT:
			y = rect.y() + ( rect.height() + fex.ascent ) / 2.0;
			break;

		case alignment::BOTTOM:
		case alignment::BOTTOM_RIGHT:
		case alignment::BOTTOM_LEFT:
			y = rect.y2() - fex.descent;
			break;

		case alignment::TOP:
		case alignment::TOP_RIGHT:
		case alignment::TOP_LEFT:
			y = rect.y1() + fex.ascent;
			break;
	}

	switch ( a )
	{
		case alignment::LEFT:
		case alignment::TOP_LEFT:
		case alignment::BOTTOM_LEFT:
			x = rect.x() - tex.x_bearing;
			break;

		case alignment::RIGHT:
		case alignment::TOP_RIGHT:
		case alignment::BOTTOM_RIGHT:
			x = rect.x2() - tex.width;
			break;

		case alignment::CENTER:
		case alignment::TOP:
		case alignment::BOTTOM:
			x = rect.x1() + ( rect.width() - tex.width ) /2.0;
			break;
	}

	return { x, y };
}

////////////////////////////////////////

void canvas::present( void )
{
}
*/

////////////////////////////////////////

void canvas::save( void )
{
	save_matrix();
}

////////////////////////////////////////

void canvas::restore( void )
{
	restore_matrix();
}

////////////////////////////////////////

}

