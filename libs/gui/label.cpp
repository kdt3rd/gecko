//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "label.h"
#include "application.h"

namespace gui
{

////////////////////////////////////////

label::label( void )
{
}

////////////////////////////////////////

label::label( std::string l, base::alignment a, const base::color &c, const std::shared_ptr<script::font> &f )
	: _text( std::move( l ) ), _align( a ), _color( c ), _font( f )
{
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::paint( const std::shared_ptr<draw::canvas> &c )
{
	base::point p = c->align_text( _font, _text, *this, _align );

	base::paint paint;
	paint.set_fill_color( _color );
	c->draw_text( _font, p, _text, paint );
}

////////////////////////////////////////

void label::compute_minimum( void )
{
	script::font_extents fex = _font->extents();
	script::text_extents tex = _font->extents( _text );
	set_minimum( tex.x_advance + 12, fex.height );
}

////////////////////////////////////////

}

