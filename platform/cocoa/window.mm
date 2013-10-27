
#include "window.h"
#include <draw/cairo/canvas.h>
#include <core/pointer.h>

#include <iostream>

#include <core/contract.h>
#include <stdexcept>

#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>

namespace cocoa
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

void window::resized( double w, double h )
{
	update_canvas( w, h );
	platform::window::resized( w, h );
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

std::shared_ptr<draw::canvas> window::canvas( void )
{
	if ( !_canvas )
	{
		_canvas = std::make_shared<cairo::canvas>();

//		auto cookie = xcb_get_geometry( _connection, _win );
//		auto geom = core::wrap_cptr( xcb_get_geometry_reply( _connection, cookie, nullptr ) );
//		update_canvas( geom->width, geom->height );
	}
	return _canvas;
}

////////////////////////////////////////

void window::update_canvas( double ww, double hh )
{
	if ( !_canvas )
		return;

	uint32_t w = uint32_t(ww+0.5);
	uint32_t h = uint32_t(hh+0.5);

	if ( w > 0 && h > 0 )
	{
		if ( _canvas->has_surface() )
			_canvas->set_size( w, h );
//		else
//			_canvas->set_surface( cairo_xcb_surface_create( _connection, _win, _visual, w, h ) );
	}
	else
		_canvas->clear_surface();
}

////////////////////////////////////////

}

