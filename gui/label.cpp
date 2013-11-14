
#include "label.h"
#include "application.h"
#include "style.h"

namespace gui
{

////////////////////////////////////////

label::label( const std::string &t )
{
	_text = t;
	_font = application::current()->get_style()->default_font();
}

////////////////////////////////////////

label::label( void )
{
	_font = application::current()->get_style()->default_font();
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::paint( const std::shared_ptr<draw::canvas> &c )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );

	draw::rect a = rectangle();
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
	c->draw_text( _font, { x, y }, _text, paint );
}

////////////////////////////////////////

void label::recompute_minimum( void )
{
	draw::font_extents fex = _font->extents();
	draw::text_extents tex = _font->extents( _text );

	set_minimum( tex.width - tex.x_bearing, fex.height );
}

////////////////////////////////////////

}

