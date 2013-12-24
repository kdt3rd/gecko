
#include <iostream>
#include "label.h"
#include "application.h"
#include "style.h"
#include <reaction/passive.h>

namespace gui
{

////////////////////////////////////////

label::label( const std::string &t, alignment a )
	: _align( a )
{
	_text = t;
	_font = application::current()->get_style()->default_font();
	_action.reset( new reaction::passive );
}

////////////////////////////////////////

label::label( void )
{
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::paint( const std::shared_ptr<draw::canvas> &c )
{
	draw::point p = c->align_text( _font, _text, *this, _align );

	draw::paint paint;
	paint.set_fill_color( _color );
	c->draw_text( _font, p, _text, paint );
}

////////////////////////////////////////

void label::compute_minimum( void )
{
	auto style = application::current()->get_style();
	auto font = style->default_font( false );

	draw::font_extents fex = font->extents();
	draw::text_extents tex = font->extents( _text );
	set_minimum( tex.x_advance + 12, fex.height );
}

////////////////////////////////////////

}

