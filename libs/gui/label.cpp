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

label::label( std::string l, base::alignment a, const gl::color &c, const std::shared_ptr<script::font> &f )
	: _align( a )
{
	_text.set_font( f );
	_text.set_text( l );
	_text.set_color( c );
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::paint( gl::api &ogl )
{
	const auto &f = _text.get_font();
	if ( f )
	{
		_text.set_position( f->align_text( _text.get_text(), *this, _align ) );
		_text.draw( ogl );
	}
}

////////////////////////////////////////

void label::compute_bounds( void )
{
	const auto &f = _text.get_font();
	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	set_minimum( tex.x_advance + 12, fex.height );
}

////////////////////////////////////////

}

