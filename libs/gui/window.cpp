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
	_window->resized = [this] ( double w, double h ) { resized( w, h ); };
	_window->mouse_pressed = [this]( const std::shared_ptr<platform::mouse> &, const base::point &p, int b ) { mouse_press( p, b ); };
	_window->mouse_released = [this]( const std::shared_ptr<platform::mouse> &, const base::point &p, int b ) { mouse_release( p, b ); };
	_window->mouse_moved = [this]( const std::shared_ptr<platform::mouse> &, const base::point &p ) { mouse_moved( p ); };
	_window->mouse_wheel = [this]( const std::shared_ptr<platform::mouse> &, int i ) { mouse_wheel( i ); };
	_window->key_pressed = [this]( const std::shared_ptr<platform::keyboard> &, const platform::scancode &c ) { key_pressed( c ); };
	_window->key_released = [this]( const std::shared_ptr<platform::keyboard> &, const platform::scancode &c ) { key_released( c ); };
	_window->text_entered = [this]( const std::shared_ptr<platform::keyboard> &, const char32_t &c ) { text_entered( c ); };
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

void window::move( double x, double y )
{
	_window->move( x, y );
}

////////////////////////////////////////

void window::resize( double w, double h )
{
	_window->resize( w, h );
}

////////////////////////////////////////

void window::set_widget( const std::shared_ptr<widget> &w )
{
	in_context( [&,this]
	{
		_widget = w;
		_widget->set_horizontal( 0.0, _window->width() - 1.0 );
		_widget->set_vertical( 0.0, _window->height() - 1.0 );
		_widget->compute_bounds();
	} );
}

////////////////////////////////////////

double window::width( void ) const
{
	return _window->width();
}

////////////////////////////////////////

double window::height( void ) const
{
	return _window->height();
}

////////////////////////////////////////

void window::invalidate( const base::rect &r )
{
	_window->invalidate( r );
}

////////////////////////////////////////

window::bound_context window::bind( void )
{
	_window->acquire();
	return bound_context( [=]( void ) { _window->release(); } );
}

////////////////////////////////////////

void window::paint( void )
{
	_window->acquire();

	double w = _window->width();
	double h = _window->height();

	_ogl.reset();
	_ogl.viewport( 0, 0, w, h );
	_ogl.enable( gl::capability::MULTISAMPLE );

	_ogl.clear_color( { 0.15, 0.15, 0.15, 1 } );
	_ogl.clear();
	_ogl.set_projection( gl::matrix4::ortho( 0, w, 0, h ) );

	if ( _widget )
	{
		in_context( [&,this]
		{
			_widget->compute_bounds();
			_widget->set_size( w, h );
			_widget->compute_layout();
			_widget->paint( _ogl );
		} );
	}

	_window->release();
}

////////////////////////////////////////

void window::resized( double w, double h )
{
	if ( _widget )
		in_context( [&,this]
		{
			_widget->set_horizontal( 0.0, w - 1.0 );
			_widget->set_vertical( 0.0, h - 1.0 );
			_widget->compute_layout();
			_widget->invalidate();
		} );
}

////////////////////////////////////////

void window::mouse_press( const base::point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_press( p, b ); } );
}

////////////////////////////////////////

void window::mouse_release( const base::point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_release( p, b ); } );
}

////////////////////////////////////////

void window::mouse_moved( const base::point &p )
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

