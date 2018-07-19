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

	using etype = script::extent_type;
	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	const coord ht = std::max( s.widget_minimum_size().h(), ctxt.from_native_vert( fex.height + etype(2) ) );
	const coord wt = ctxt.from_native_horiz( tex.width + etype(10) );
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
//		rect lbox = *this;
//		lbox.shrink( ctxt.from_native_horiz( 10 ), ctxt.from_native_horiz( 10 ),
//					 ctxt.from_native_vert( 5 ), ctxt.from_native_vert( 5 ) );
//		_text.set_position( f->align_text( _text.get_text(), lbox, _align ) );

		auto pix = ctxt.to_native( *this );
		pix.shrink( 10, 10, 5, 5 );
		using et = script::extent_type;
		auto pos = f->align_text( _text.get_text(),
								  static_cast<et>( pix.x1() ), static_cast<et>( pix.y1() ),
								  static_cast<et>( pix.x2() ), static_cast<et>( pix.y2() ),
								  _align );
		auto outp = ctxt.from_native(
			platform::point(
				static_cast<platform::coord_type>( pos.first + et(0.5) ),
				static_cast<platform::coord_type>( pos.second + et(0.5) ) ) );

		auto scalep = ctxt.from_native(
			platform::point( platform::coord_type(1), platform::coord_type(1) ) );

		_text.set_scale( scalep );
		_text.set_position( outp );
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
