
#include <iostream>
#include "label.h"
#include "application.h"
#include "style.h"

namespace gui
{

////////////////////////////////////////

label::label( void )
{
	callback_invalidate( _text, _align, _color, _font );
}

////////////////////////////////////////

label::label( datum<std::string> &&l, datum<alignment> &&a, datum<core::color> &&c, shared_datum<draw::font> &&f )
	: _text( std::move( l ) ), _align( std::move( a ) ), _color( std::move( c ) ), _font( std::move( f ) )
{
	callback_invalidate( _text, _align, _color, _font );
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::paint( const std::shared_ptr<draw::canvas> &c )
{
//	core::point p = c->align_text( _font.value(), _text.value(), *this, _align.value() );

//	draw::paint paint;
//	paint.set_fill_color( _color.value() );
//	c->draw_text( _font.value(), p, _text.value(), paint );
}

////////////////////////////////////////

void label::compute_minimum( void )
{
	draw::font_extents fex;// = _font.value()->extents();
	draw::text_extents tex;// = _font.value()->extents( _text.value() );
	set_minimum( tex.x_advance + 12, fex.height );
}

////////////////////////////////////////

}

