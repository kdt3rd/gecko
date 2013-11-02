
#include "text.h"

////////////////////////////////////////

namespace view
{

////////////////////////////////////////

text::text( const std::shared_ptr<draw::area> &a )
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
	draw::font_extents fex = canvas->font_extents( _font );
	draw::text_extents tex = canvas->text_extents( _font, _text );

	_area->set_minimum( tex.width - tex.x_bearing, fex.height );
}

////////////////////////////////////////

void text::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::font_extents fex = canvas->font_extents( _font );
	draw::text_extents tex = canvas->text_extents( _font, _text );

	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, { _area->x() - tex.x_bearing, _area->y() + fex.ascent }, _text, paint );
}

////////////////////////////////////////

}

