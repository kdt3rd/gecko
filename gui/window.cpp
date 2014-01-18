
#include <iostream>
#include "window.h"
#include "application.h"
#include "style.h"
#include <platform/window.h>
#include <gl/opengl.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &w )
	: _window( w )
{
	precondition( bool(_window), "null window" );
	_window->exposed.callback( [this] ( void ) { paint(); } );
	_window->resized.callback( [this] ( double w, double h ) { resize( w, h ); } );
	_window->mouse_pressed.callback( [this]( const std::shared_ptr<platform::mouse> &, const core::point &p, int b ) { mouse_press( p, b ); } );
	_window->mouse_released.callback( [this]( const std::shared_ptr<platform::mouse> &, const core::point &p, int b ) { mouse_release( p, b ); } );
	_window->mouse_moved.callback( [this]( const std::shared_ptr<platform::mouse> &, const core::point &p ) { mouse_moved( p ); } );
	_window->key_pressed.callback( [this]( const std::shared_ptr<platform::keyboard> &, const platform::scancode &c ) { key_pressed( c ); } );
	_window->key_released.callback( [this]( const std::shared_ptr<platform::keyboard> &, const platform::scancode &c ) { key_released( c ); } );
	_window->text_entered.callback( [this]( const std::shared_ptr<platform::keyboard> &, const char32_t &c ) { text_entered( c ); } );
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

void window::set_widget( const std::shared_ptr<widget> &w )
{
	in_context( [&,this]
	{
		_widget = w;
		_widget->set_horizontal( 0.0, _window->width() - 1.0 );
		_widget->set_vertical( 0.0, _window->height() - 1.0 );
		_widget->compute_minimum();
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

void window::invalidate( const core::rect &r )
{
	_window->invalidate( r );
}

////////////////////////////////////////

void window::paint( void )
{
	auto canvas = _window->canvas();
	glViewport( 0, 0, _window->width(), _window->height() );
	auto style = application::current()->get_style();
	if ( style )
		style->background( canvas );
	if ( _widget )
		in_context( [&,this]
		{
			_widget->compute_minimum();
			_widget->compute_layout();
			_widget->paint( canvas );
		} );
}

////////////////////////////////////////

void window::resize( double w, double h )
{
	if ( _widget )
		in_context( [&,this]
		{
			_widget->set_horizontal( 0.0, w - 1.0 );
			_widget->set_vertical( 0.0, h - 1.0 );
			_widget->compute_layout();
		} );
}

////////////////////////////////////////

void window::mouse_press( const core::point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_press( p, b ); } );
}

////////////////////////////////////////

void window::mouse_release( const core::point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_release( p, b ); } );
}

////////////////////////////////////////

void window::mouse_moved( const core::point &p )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_move( p ); } );
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

