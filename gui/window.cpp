
#include <iostream>
#include "window.h"
#include "application.h"
#include "style.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &w )
	: _window( w )
{
	precondition( bool(_window), "null window" );
	_window->exposed.callback( [=] { this->paint(); } );
	_window->resized.callback( [=]( double w, double h ) { this->resize( w, h ); } );
	_window->mouse_pressed.callback( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p, int b ) { this->mouse_press( p, b ); } );
	_window->mouse_released.callback( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p, int b ) { this->mouse_release( p, b ); } );
	_window->mouse_moved.callback( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p ) { this->mouse_moved( p ); } );
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
	_widget = w;
	_widget->set_horizontal( 0.0, _window->width() - 1.0 );
	_widget->set_vertical( 0.0, _window->height() - 1.0 );
	_widget->compute_minimum();
	_widget->set_delegate( this );
}

////////////////////////////////////////

void window::invalidate( const draw::rect &r )
{
	_window->invalidate( r );
}

////////////////////////////////////////

void window::paint( void )
{
	auto canvas = _window->canvas();
	auto style = application::current()->get_style();
	if ( style )
		style->background( canvas );
	if ( _widget )
		_widget->paint( canvas );
}

////////////////////////////////////////

void window::resize( double w, double h )
{
	if ( _widget )
	{
		_widget->set_horizontal( 0.0, w - 1.0 );
		_widget->set_vertical( 0.0, h - 1.0 );
		_widget->compute_layout();
	}
}

////////////////////////////////////////

void window::mouse_press( const draw::point &p, int b )
{
	if ( _widget )
		_widget->mouse_press( p, b );
}

////////////////////////////////////////

void window::mouse_release( const draw::point &p, int b )
{
	if ( _widget )
		_widget->mouse_release( p, b );
}

////////////////////////////////////////

void window::mouse_moved( const draw::point &p )
{
	if ( _widget )
		_widget->mouse_move( p );
}

////////////////////////////////////////

}

