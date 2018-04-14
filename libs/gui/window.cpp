//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "window.h"
#include "widget.h"
#include <platform/window.h>
#include <base/contract.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &win )
	: _window( win )
{
	precondition( bool(_window), "null window" );
	_window->exposed = [this] ( void ) { paint(); };
	_window->resized = [this] ( platform::coord_type w, platform::coord_type h ) { resized( coord_type( w ), coord_type( h ) ); };
	_window->mouse_pressed = [this]( platform::event_source &, const platform::point &p, int b ) { mouse_press( point( p ), b ); };
	_window->mouse_released = [this]( platform::event_source &, const platform::point &p, int b ) { mouse_release( point( p ), b ); };
	_window->mouse_moved = [this]( platform::event_source &, const platform::point &p ) { mouse_moved( point( p ) ); };
	_window->mouse_wheel = [this]( platform::event_source &, int i ) { mouse_wheel( i ); };
	_window->key_pressed = [this]( platform::event_source &, const platform::scancode &c ) { key_pressed( c ); };
	_window->key_released = [this]( platform::event_source &, const platform::scancode &c ) { key_released( c ); };
	_window->text_entered = [this]( platform::event_source &, const char32_t &c ) { text_entered( c ); };
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	_window->set_title( t );
}

////////////////////////////////////////

void
window::set_default_cursor( const std::shared_ptr<platform::cursor> &c )
{
	_window->set_default_cursor( c );
}

////////////////////////////////////////

void
window::push_cursor( const std::shared_ptr<platform::cursor> &c )
{
	_window->push_cursor( c );
}

////////////////////////////////////////

void
window::pop_cursor( void )
{
	_window->pop_cursor();
}

////////////////////////////////////////

void window::show( void )
{
	_window->show();
}

////////////////////////////////////////

void window::hide( void )
{
	_window->hide();
}

////////////////////////////////////////

void window::move( coord_type x, coord_type y )
{
	_window->move( x, y );
}

////////////////////////////////////////

void window::resize( coord_type w, coord_type h )
{
	_window->resize( w, h );
}

////////////////////////////////////////

void window::set_widget( const std::shared_ptr<widget> &w )
{
	in_context( [&,this]
	{
		_widget = w;
		_widget->build( *this );
		_widget->layout_target()->compute_bounds();
		_widget->layout_target()->set_horizontal( 0.0, _window->width() - 1.0 );
		_widget->layout_target()->set_vertical( 0.0, _window->height() - 1.0 );
	} );
}

////////////////////////////////////////

coord_type window::width( void ) const
{
	return _window->width();
}

////////////////////////////////////////

coord_type window::height( void ) const
{
	return _window->height();
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
	_window->invalidate( platform::rect( r.round() ) );
}

////////////////////////////////////////

platform::context &window::hw_context( void )
{
	return _window->hw_context();
}

////////////////////////////////////////

platform::context::render_guard window::bind( void )
{
	return _window->hw_context().begin_render();
}

////////////////////////////////////////

void window::paint( void )
{
	coord_type w = _window->width();
	coord_type h = _window->height();

	platform::context &hwctxt = _window->hw_context();
	gl::api &ogl = hwctxt.api();
	ogl.reset();
	ogl.viewport( 0, 0, w, h );
	ogl.enable( gl::capability::MULTISAMPLE );
	ogl.enable( gl::capability::BLEND );
    ogl.blend_func( gl::blend_style::SRC_ALPHA, gl::blend_style::ONE_MINUS_SRC_ALPHA );

	ogl.clear_color( _style.background_color() );
	ogl.clear();
	ogl.set_projection( gl::matrix4::ortho( 0, w, 0, h ) );

	if ( _widget )
	{
		in_context( [&,this]
		{
			_widget->layout_target()->compute_bounds();
			_widget->set_size( w, h );
			_widget->layout_target()->set_size( w, h );
			_widget->layout_target()->compute_layout();
			if ( _widget->update_layout( 250.0 ) )
				invalidate( *_widget );
			_widget->paint( *this );
		} );
	}
}

////////////////////////////////////////

void window::resized( coord_type w, coord_type h )
{
	if ( _widget )
	{
		in_context( [&,this]
		{
			_widget->set( { 0.0, 0.0 }, { w, h } );
			_widget->layout_target()->set( { 0.0, 0.0 }, { w, h } );
			_widget->layout_target()->compute_layout();
			_widget->update_layout( 0.0 );
			invalidate( *_widget );
		} );
	}
}

////////////////////////////////////////

void window::mouse_press( const point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_press( p, b ); } );
}

////////////////////////////////////////

void window::mouse_release( const point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_release( p, b ); } );
}

////////////////////////////////////////

void window::mouse_moved( const point &p )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_move( p ); } );
}

////////////////////////////////////////

void window::mouse_wheel( int amount )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_wheel( amount ); } );
}

////////////////////////////////////////

void window::key_pressed( platform::scancode c )
{
	if ( _widget )
		in_context( [&,this] { _widget->key_press( c ); } );
}

////////////////////////////////////////

void window::key_released( platform::scancode c )
{
	if ( _widget )
		in_context( [&,this] { _widget->key_release( c ); } );
}

////////////////////////////////////////

void window::text_entered( char32_t c )
{
	if ( _widget )
		in_context( [&,this] { _widget->text_input( c ); } );
}

////////////////////////////////////////

}

