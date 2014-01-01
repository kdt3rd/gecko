
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

	int black = BlackPixel( _display, DefaultScreen( _display ) );
	_win = XCreateSimpleWindow( _display, DefaultRootWindow( _display ), 0, 0, 320, 240, 0, black, black ); 

	long events =
		ExposureMask | StructureNotifyMask | VisibilityChangeMask |
		EnterWindowMask | LeaveWindowMask |
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask | ButtonMotionMask;
	XSelectInput( _display, _win, events );
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
	XRaiseWindow( _display, _win );
}

////////////////////////////////////////

void window::lower( void )
{
	XLowerWindow( _display, _win );
}

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
	XResizeWindow( _display, _win, (unsigned int)( std::max( 0.0, w ) + 0.5 ), (unsigned int)( std::max( 0.0, h ) + 0.5 ) );
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
	XStoreName( _display, _win, t.c_str() );
}

////////////////////////////////////////

void window::invalidate( const draw::rect &r )
{
	XClearArea( _display, _win, std::floor( r.x() ), std::floor( r.y() ), std::ceil( r.width() ), std::ceil( r.height() ), true );
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

