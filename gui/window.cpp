
#include <iostream>
#include "window.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &w )
	: _window( w )
{
	precondition( bool(_window), "null window" );
	_area = std::make_shared<draw::area>( _window->width(), _window->height() );
	_container = std::make_shared<container>( _area );
	_container->set_delegate( this );
	_window->exposed.callback( [=] { this->paint(); } );
	_window->resized.callback( [=]( double w, double h ) { this->resize( w, h ); } );
	_window->mouse_pressed.callback( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p, int b ) { this->mouse_press( p, b ); } );
	_window->mouse_released.callback( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p, int b ) { this->mouse_release( p, b ); } );
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

void window::invalidate( const draw::rect &r )
{
	_window->invalidate( r );
}

////////////////////////////////////////

void window::paint( void )
{
	auto canvas = _window->canvas();

	_container->layout( canvas );
	_container->paint( canvas );
}

////////////////////////////////////////

void window::resize( double w, double h )
{
	_area->set_horizontal( 0.0, w - 1.0 );
	_area->set_vertical( 0.0, h - 1.0 );
	_area->set_minimum( w, h );
}

////////////////////////////////////////

void window::mouse_press( const draw::point &p, int b )
{
	_container->mouse_press( p, b );
}

////////////////////////////////////////

void window::mouse_release( const draw::point &p, int b )
{
	_container->mouse_release( p, b );
}

////////////////////////////////////////

}

