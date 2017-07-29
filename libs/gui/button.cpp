//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "button.h"
#include "application.h"


namespace gui
{

////////////////////////////////////////

button::button( void )
{
}

////////////////////////////////////////

button::button( std::string l, base::alignment a, const gl::color &c, const std::shared_ptr<script::font> &f )
	: _align( a )
{
	_text.set_font( f );
	_text.set_text( l );
	_text.set_color( c );
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
			_rect.set_color( { 0.05, 0.05, 0.05 } );
		else
			_rect.set_color( { 0.25, 0.25, 0.25 } );
		invalidate();
	}
}

////////////////////////////////////////

void button::paint( gl::api &ogl )
{
	_rect.resize( x(), y(), width(), height() );
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
	set_minimum( tex.x_advance + 20, fex.height + 10 );
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

