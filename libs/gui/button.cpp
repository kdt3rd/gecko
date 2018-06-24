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

button_w::button_w( void )
	: _rect( { 0.26, 0.26, 0.26 } )
{
}

////////////////////////////////////////

button_w::button_w( std::string l, base::alignment a )
	: _align( a )
{
	gl::color bg = context::current().get_style().dominant_color();
	_rect.set_color( bg );
	_text.set_text( l );
	_text.set_color( context::current().get_style().primary_text( bg ) );
}

////////////////////////////////////////

button_w::~button_w( void )
{
}

////////////////////////////////////////

void button_w::set_pressed( bool p )
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

void button_w::build( context &ctxt )
{
	style &s = ctxt.get_style();
	const auto &f = s.body_font();

	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	layout_target()->set_minimum( tex.width + 10.0, std::max( 24.0, fex.height + 2.0 ) );
	layout_target()->set_maximum_height( 24 );

	_text.set_font( f );
}

////////////////////////////////////////

void button_w::paint( context &ctxt )
{
	platform::context &hwc = ctxt.hw_context();
	_rect.set_position( x(), y() );
	_rect.set_size( width(), height() );
	_rect.draw( hwc );

	const auto &f = _text.get_font();
	if ( f )
	{
		rect lbox = *this;
		lbox.shrink( 10, 10, 5, 5 );
		_text.set_position( f->align_text( _text.get_text(), lbox, _align ) );
		_text.draw( hwc );
	}
}

////////////////////////////////////////

bool button_w::mouse_press( const event &e )
{
	if ( e.mouse().button != 1 )
		return false;

	context::current().grab_source( e, shared_from_this() );

	_tracking = true;
	set_pressed( true );

	return true;
}

////////////////////////////////////////

bool button_w::mouse_release( const event &e )
{
	if ( e.mouse().button != 1 )
		return false;

	if ( _tracking )
	{
		on_scope_exit{ context::current().release_source( e ); };
		_tracking = false;
		set_pressed( false );
		if ( contains( e.mouse().x, e.mouse().y ) )
			when_activated();
		return true;
	}

	return false;
}

////////////////////////////////////////

bool button_w::mouse_move( const event &e )
{
	if ( _tracking )
	{
		set_pressed( contains( e.mouse().x, e.mouse().y ) );
		return true;
	}
	return false;
}

////////////////////////////////////////

}

