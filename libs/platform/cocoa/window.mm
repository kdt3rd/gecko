
#include "window.h"
#include <base/pointer.h>

#include <iostream>

#include <base/contract.h>
#include <stdexcept>

#include <Cocoa/Cocoa.h>

namespace platform { namespace cocoa
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
	_canvas = std::make_shared<draw::canvas>();
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

void window::set_popup( void )
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
	_last_w = w;
	_last_h = h;
	if ( _canvas )
		_impl->ctxt = nullptr;
	resized( w, h );
}

////////////////////////////////////////

void window::invalidate( const base::rect &r )
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
	[[_impl->view openGLContext] makeCurrentContext];
	return _canvas;
}

////////////////////////////////////////

void window::set_ns( void *w, void *v )
{
	_impl->win = (NSWindow *)w;
	_impl->view = (NSOpenGLView *)v;
	if ( [ _impl->view respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:) ] ) 
		[ _impl->view setWantsBestResolutionOpenGLSurface:YES ]; 
}

////////////////////////////////////////

} }

