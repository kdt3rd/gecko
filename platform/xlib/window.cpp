
#include "window.h"
#include <draw/cairo/canvas.h>
#include <core/pointer.h>

#include <iostream>

#include <core/contract.h>
#include <stdexcept>

#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

namespace xlib
{

////////////////////////////////////////

window::window( Display *dpy )
	: _display( dpy )
{
	precondition( _display, "null display" );
//	precondition( _screen, "null screen" );

	int black = BlackPixel( _display, DefaultScreen( _display ) );
	_win = XCreateSimpleWindow( _display, DefaultRootWindow( _display ), 0, 0, 320, 240, 0, black, black ); 

	long events =
		ExposureMask | StructureNotifyMask | VisibilityChangeMask |
		EnterWindowMask | LeaveWindowMask |
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask | ButtonMotionMask;
	XSelectInput( _display, _win, events );

	/*
	const uint32_t values[] = { screen->black_pixel };
	xcb_change_window_attributes( _connection, _win, XCB_CW_BACK_PIXEL, values );

	auto depth_iter = xcb_screen_allowed_depths_iterator( screen );
	while ( depth_iter.rem )
	{
		auto visual_iter = xcb_depth_visuals_iterator( depth_iter.data );
		while ( visual_iter.rem )
		{
			if ( screen->root_visual == visual_iter.data->visual_id )
			{
				_visual = visual_iter.data;
				break;
			}
			xcb_visualtype_next( &visual_iter );
		}
		xcb_depth_next( &depth_iter );
	}

	postcondition( _visual, "visual not found" );
	*/
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
//	const static uint32_t values[] = { XCB_STACK_MODE_ABOVE };
//	xcb_configure_window( _connection, _win, XCB_CONFIG_WINDOW_STACK_MODE, values);
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
	XMapWindow( _display, _win );
}

////////////////////////////////////////

void window::hide( void )
{
	XUnmapWindow( _display, _win );
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
//	const static uint32_t values[] = { uint32_t(w+0.5), uint32_t(h+0.5) };
//	xcb_configure_window( _connection, _win, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values );
//	resize_canvas( w, h );
}

////////////////////////////////////////

void window::resize_canvas( double w, double h )
{
	update_canvas( w, h );
	resized( w, h );
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
//	xcb_change_property( _connection, XCB_PROP_MODE_REPLACE, _win, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, t.size(), t.c_str() );
}

////////////////////////////////////////

void window::invalidate( const draw::rect &r )
{
//	exposed();
//	xcb_flush( _connection );
//	xcb_clear_area( _connection, 1, _win, std::floor( r.x() ), std::floor( r.y() ), std::ceil( r.width() ), std::ceil( r.height() ) );
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	if ( !_canvas )
	{
		_canvas = std::make_shared<cairo::canvas>();

		Window root;
		int x, y;
		unsigned int w, h;
		unsigned int border, depth;
		XGetGeometry( _display, _win, &root, &x, &y, &w, &h, &border, &depth );
		update_canvas( w, h );
	}
	return _canvas;
}

////////////////////////////////////////

Window window::id( void ) const
{
	return _win;
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
		{
			cairo_xlib_surface_set_size( _canvas->get_surface(), w, h );
			cairo_surface_flush( _canvas->get_surface() );
		}
		else
			_canvas->set_surface( cairo_xlib_surface_create( _display, _win, DefaultVisual( _display, DefaultScreen( _display ) ), w, h ) );
	}
	else
		_canvas->clear_surface();
}

////////////////////////////////////////

}

