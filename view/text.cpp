
#include "text.h"

////////////////////////////////////////

namespace view
{

////////////////////////////////////////

text::text( const std::shared_ptr<draw::area> &a )
	: _area( a )
{
	_area->set_minimum( 100, 21 );
}

////////////////////////////////////////

text::~text( void )
{
}

////////////////////////////////////////

void text::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::paint paint;
	paint.set_fill_color( _color );
	canvas->draw_text( _font, { _area->x() + 18, _area->y() + 16 }, _text, paint );
}

////////////////////////////////////////

}

