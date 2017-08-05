//
// Copyright (c) 2014-2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "button.h"
#include "application.h"
#include <draw/path.h>
#include <draw/paint.h>

namespace gui
{

////////////////////////////////////////

button::button( void )
	: _rect( { 0.26, 0.26, 0.26 } )
{
	set_maximum_height( 24 );
}

////////////////////////////////////////

button::button( std::string l, base::alignment a, const std::shared_ptr<script::font> &f )
	: _align( a )
{
	gl::color bg = context::current().get_style().dominant_color();
	_rect.set_color( bg );
	_text.set_font( f );
	_text.set_text( l );
	_text.set_color( context::current().get_style().primary_text( bg ) );
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

void button::set_pressed( bool p )
{
	if ( p != _pressed )
	{
		_pressed = p;
		if ( _pressed )
			_rect.set_color( context::current().get_style().dominant_light() );
		else
			_rect.set_color( context::current().get_style().dominant_color() );
		invalidate();
	}
}

////////////////////////////////////////

void button::build( gl::api &ogl )
{
}

////////////////////////////////////////

void button::paint( gl::api &ogl )
{
	_rect.set_position( x(), y() );
	_rect.set_size( width(), height() );
	_rect.draw( ogl );

	const auto &f = _text.get_font();
	if ( f )
	{
		base::rect lbox = *this;
		lbox.shrink( 10, 10, 5, 5 );
		_text.set_position( f->align_text( _text.get_text(), lbox, _align ) );
		_text.draw( ogl );
	}
}

////////////////////////////////////////

void button::compute_bounds( void )
{
	const auto &f = _text.get_font();
	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	set_minimum( tex.x_advance + 10.0, std::max( 24.0, fex.height + 2.0 ) );
}

////////////////////////////////////////

bool button::mouse_press( const base::point &p, int button )
{
	unused( button );

	if ( contains( p ) )
	{
		_tracking = true;
		set_pressed( true );
	}
	return _tracking;
}

////////////////////////////////////////

bool button::mouse_release( const base::point &p, int button )
{
	unused( button );

	if ( _tracking )
	{
		_tracking = false;
		set_pressed( false );
		if ( contains( p ) )
			when_activated();
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_move( const base::point &p )
{
	if ( _tracking )
	{
		set_pressed( contains( p ) );
		return true;
	}
	return false;
}

////////////////////////////////////////

}

