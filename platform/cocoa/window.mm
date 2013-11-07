
#include "window.h"
#include <draw/cairo/canvas.h>
#include <core/pointer.h>

#include <iostream>

#include <core/contract.h>
#include <stdexcept>

#include <Cocoa.h>

#include <cairo.h>
#include <cairo-quartz.h>

namespace cocoa
{

struct window::objcwrapper
{
	NSWindow *win;
	CGContext *ctxt;
};

////////////////////////////////////////

window::window( void )
	: _impl( new objcwrapper )
{
	_impl->win = nullptr;
	_impl->ctxt = nullptr;
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
	[_impl->win makeKeyAndOrderFront:nil];
}

////////////////////////////////////////

void window::hide( void )
{
	[_impl->win orderOut:nil];
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
	_last_w = w;
	_last_h = h;
	_canvas.reset();
/*
	if ( _canvas )
	{
		if ( _canvas->has_surface() )
		{
			cairo_surface_t *surf = _canvas->get_surface();
//			_canvas->clear_surface();
			cairo_surface_destroy( _canvas->get_surface() );
		}
		CGContextTranslateCTM( _impl->ctxt, 0.0, _last_h );
		CGContextScaleCTM( _impl->ctxt, 1.0, -1.0 );
		_canvas->set_surface( cairo_quartz_surface_create_for_cg_context( _impl->ctxt, _last_w, _last_h ) );
	}
*/
	platform::window::resized( w, h );
}

////////////////////////////////////////

void window::exposed( void )
{
	std::cout << "EXPOSED" << std::endl;
	platform::window::exposed();
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	NSString *tstring = [[NSString alloc] initWithUTF8String:t.c_str()];
	[_impl->win setTitle:tstring];
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	if ( !_canvas )
	{
		_canvas = std::make_shared<cairo::canvas>();
		NSGraphicsContext *nsgctxt = [_impl->win graphicsContext];
		_impl->ctxt = (CGContextRef)[nsgctxt graphicsPort];
		CGContextTranslateCTM( _impl->ctxt, 0.0, _last_h );
		CGContextScaleCTM( _impl->ctxt, 1.0, -1.0 );
		std::cout << "Created with context: " << (void*)_impl->ctxt << std::endl;
		_canvas->set_surface( cairo_quartz_surface_create_for_cg_context( _impl->ctxt, _last_w, _last_h ) );
	}
	return _canvas;
}

////////////////////////////////////////

void window::set_ns_window( void *w )
{
	_impl->win = (NSWindow *)w;
}

////////////////////////////////////////

}

