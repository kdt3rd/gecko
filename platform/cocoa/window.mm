
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
	NSView *view;
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

void window::lower( void )
{
}

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
	if ( _canvas )
	{
		_canvas->clear_surface();
		_impl->ctxt = nullptr;
	}
	platform::window::resized( w, h );
}

////////////////////////////////////////

void window::exposed( void )
{
	platform::window::exposed();
}

////////////////////////////////////////

void window::invalidate( const draw::rect &r )
{
	[_impl->view setNeedsDisplay:YES];
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
		_canvas = std::make_shared<cairo::canvas>();

	if ( _canvas->get_surface() == nullptr )
	{
		NSGraphicsContext *nsgctxt = [_impl->win graphicsContext];
		_impl->ctxt = (CGContextRef)[nsgctxt graphicsPort];
//		CGContextTranslateCTM( _impl->ctxt, 0.0, _last_h );
//		CGContextScaleCTM( _impl->ctxt, 1.0, -1.0 );
		_canvas->set_surface( cairo_quartz_surface_create_for_cg_context( _impl->ctxt, _last_w, _last_h ) );
	}

	return _canvas;
}

////////////////////////////////////////

void window::set_ns( void *w, void *v )
{
	_impl->win = (NSWindow *)w;
	_impl->view = (NSView *)v;
}

////////////////////////////////////////

}

