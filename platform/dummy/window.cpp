
#include "window.h"

#include <iostream>
#include <stdexcept>

#include <core/pointer.h>
#include <core/contract.h>

namespace dummy
{

////////////////////////////////////////

window::window( void )
{
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
}

////////////////////////////////////////

/*
void window::lower( void )
{
	const static uint32_t values[] = { XCB_STACK_MODE_BELOW };
	xcb_configure_window( _connection, _win, XCB_CONFIG_WINDOW_STACK_MODE, values);
}
*/

////////////////////////////////////////

void window::show( void )
{
}

////////////////////////////////////////

void window::hide( void )
{
}

////////////////////////////////////////

bool window::is_visible( void )
{
	// TODO fix this
	return true;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::resize( double w, double h )
{
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
}

////////////////////////////////////////

void window::invalidate( const draw::rect & )
{
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
//	if ( !_canvas )
//		_canvas = std::make_shared<dummy::canvas>();
	return _canvas;
}

////////////////////////////////////////

}

