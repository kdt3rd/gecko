
#include "text.h"

////////////////////////////////////////

namespace view
{

////////////////////////////////////////

text::text( const std::shared_ptr<layout::area> &a )
	: _area( a )
{
}

////////////////////////////////////////

text::~text( void )
{
}

////////////////////////////////////////

void text::layout( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );

	_area->set_minimum( tex.width - tex.x_bearing, fex.height );
}

////////////////////////////////////////

void text::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );

	draw::rect a = _area->rectangle();
	double y = 0.0, x = 0.0;

	switch ( _align )
	{
		case alignment::CENTER:
		case alignment::LEFT:
		case alignment::RIGHT:
			y = a.y() + ( a.height() + fex.ascent ) / 2.0;
			break;

		case alignment::BOTTOM:
		case alignment::BOTTOM_RIGHT:
		case alignment::BOTTOM_LEFT:
			y = a.y2() - fex.descent;
			break;

		case alignment::TOP:
		case alignment::TOP_RIGHT:
		case alignment::TOP_LEFT:
			y = a.y1() + fex.ascent;
			break;
	}

	switch ( _align )
	{
		case alignment::LEFT:
		case alignment::TOP_LEFT:
		case alignment::BOTTOM_LEFT:
			x = a.x() - tex.x_bearing;
			break;

		case alignment::RIGHT:
		case alignment::TOP_RIGHT:
		case alignment::BOTTOM_RIGHT:
			x = a.x2() - tex.width;
			break;

		case alignment::CENTER:
		case alignment::TOP:
		case alignment::BOTTOM:
			x = a.x1() + ( a.width() - tex.width ) /2.0;
			break;

	}

	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, { x, y }, _text, paint );
}

////////////////////////////////////////

}

