
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

////////////////////////////////////////

struct window::objcwrapper
{
	NSWindow *win;
};

////////////////////////////////////////

window::window( void )
	: _impl( new objcwrapper )
{
	int style = NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask;
	NSWindow *win = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
		styleMask:style backing:NSBackingStoreBuffered defer:YES]
			autorelease];
	[win orderOut:nil];
	[win cascadeTopLeftFromPoint:NSMakePoint(20,20)];

	_impl->win = win;
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
	platform::window::resized( w, h );
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
		CGContextRef ctxt = (CGContextRef)[nsgctxt graphicsPort];
		_canvas->set_surface( cairo_quartz_surface_create_for_cg_context( ctxt, 200, 200 ) );
	}
	return _canvas;
}

////////////////////////////////////////

}

