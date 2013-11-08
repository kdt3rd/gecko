
#include "window.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &w )
	: _window( w )
{
	precondition( bool(_window), "null window" );
	_area = std::make_shared<draw::area>();
	_container = std::make_shared<container>( _area );
	_window->when_exposed( [=] { this->paint(); } );
	_window->when_resized( [=]( double w, double h ) { this->resize( w, h ); } );
	_window->when_mouse_pressed( [=]( const std::shared_ptr<platform::mouse> &, const draw::point &p, int b ) { this->mouse_press( p, b ); } );
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

}

