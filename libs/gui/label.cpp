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

label::label( std::string l, base::alignment a )
	: _align( a )
{
	_text.set_text( l );
}

////////////////////////////////////////

label::~label( void )
{
}

////////////////////////////////////////

void label::build( gl::api &ogl )
{
	const style &s = context::current().get_style();
	const auto &f = s.body_font();

	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	layout_target()->set_minimum( tex.x_advance, std::max( 24.0, fex.height + 2.0 ) );

	_text.set_font( f );
	if ( _bg_color.alpha() > 0.0 )
		_text.set_color( s.primary_text( _bg_color ) );
	else
		_text.set_color( s.primary_text( s.background_color() ) );
}

////////////////////////////////////////

void label::paint( gl::api &ogl )
{
	if ( _bg_color.alpha() > 0.0 )
	{
		_bg.set_position( x(), y() );
		_bg.set_size( width(), height() );
		_bg.draw( ogl );
	}
	const auto &f = _text.get_font();
	if ( f )
	{
		_text.set_position( f->align_text( _text.get_text(), *this, _align ) );
		_text.draw( ogl );
	}
}

////////////////////////////////////////

}

