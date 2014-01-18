
#include "window.h"
#include <core/pointer.h>

#include <iostream>

#include <core/contract.h>
#include <stdexcept>

#include <Cocoa.h>

namespace cocoa
{

struct window::objcwrapper
{
	NSWindow *win;
	NSOpenGLView *view;
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

void window::resize_event( double w, double h )
{
	std::cout << "Resized! " << w << 'x' << h << std::endl;
	_last_w = w;
	_last_h = h;
//	[[_impl->view openGLContext] makeCurrentContext];
//	glViewport( 0, 0, w, h );
	if ( _canvas )
	{
		_impl->ctxt = nullptr;
	}
	resized( w, h );
}

////////////////////////////////////////

void window::invalidate( const core::rect &r )
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

gl::context window::context( void )
{
	[[_impl->view openGLContext] makeCurrentContext];
	return gl::context();
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
//	if ( !_canvas )
//		_canvas = std::make_shared<draw::canvas>();

//	if ( _canvas->get_surface() == nullptr )
//	{
//		NSGraphicsContext *nsgctxt = [_impl->win graphicsContext];
//		_impl->ctxt = (CGContextRef)[nsgctxt graphicsPort];
//		CGContextTranslateCTM( _impl->ctxt, 0.0, _last_h );
//		CGContextScaleCTM( _impl->ctxt, 1.0, -1.0 );
//		_canvas->set_surface( cairo_quartz_surface_create_for_cg_context( _impl->ctxt, _last_w, _last_h ) );
//	}

	return _canvas;
}

////////////////////////////////////////

void window::set_ns( void *w, void *v )
{
	_impl->win = (NSWindow *)w;
	_impl->view = (NSOpenGLView *)v;
}

////////////////////////////////////////

}

