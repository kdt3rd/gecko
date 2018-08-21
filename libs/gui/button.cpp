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
	: _rect( draw::color{ 0.26F, 0.26F, 0.26F } )
{
}

////////////////////////////////////////

button_w::button_w( std::string l, base::alignment a )
	: _align( a )
{
	color bg = context::current().get_style().dominant_color();
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
	const coord ht = std::max( s.widget_minimum_size().h(), coord( fex.height ) + ctxt.from_native_vert( 4 ) );
	const coord wt = tex.width + ctxt.from_native_horiz( 10 );
	layout_target()->set_minimum( wt, ht );
	layout_target()->set_maximum_height( ht );

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
		auto horiz_off = ctxt.from_native_horiz( 5 );
		auto vert_off = ctxt.from_native_vert( 2 );
		auto pos = f->align_text( _text.get_text(), x1() + horiz_off, y1() + vert_off,
								  x2() - horiz_off, y2() - vert_off, _align );

		_text.set_position( pos.first, pos.second );
		_text.draw( hwc );
	}
}

////////////////////////////////////////

bool button_w::mouse_press( const event &e )
{
	if ( e.raw_mouse().button != 1 )
		return false;

	context::current().grab_source( e, shared_from_this() );

	_tracking = true;
	set_pressed( true );

	return true;
}

////////////////////////////////////////

bool button_w::mouse_release( const event &e )
{
	if ( e.raw_mouse().button != 1 )
		return false;

	if ( _tracking )
	{
		on_scope_exit{ context::current().release_source( e ); };
		_tracking = false;
		set_pressed( false );
		point p = e.from_native( e.raw_mouse().x, e.raw_mouse().y );
		if ( contains( p ) )
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
		set_pressed( contains( e.from_native( e.raw_mouse().x, e.raw_mouse().y ) ) );
		return true;
	}
	return false;
}

////////////////////////////////////////

}
