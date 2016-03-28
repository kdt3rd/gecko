
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
}

////////////////////////////////////////

window::~window( void )
{
	std::cout << "Popup window deleted" << std::endl;
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
//	NSUInteger mask = [_impl->win styleMask] | NSBorderlessWindowMask;
	NSUInteger mask = NSBorderlessWindowMask;
	[_impl->win setStyleMask:mask];
}

////////////////////////////////////////

void window::show( void )
{
	[_impl->win makeKeyAndOrderFront:nil];
}

////////////////////////////////////////

void window::hide( void )
{
	std::cout << "Popup window hidden" << std::endl;
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

void window::move( double x, double y )
{
	NSPoint pos;
	pos.x = x;
	pos.y = y + _last_h;
	[_impl->win setFrameOrigin:pos];
}

////////////////////////////////////////

void window::resize( double w, double h )
{
	NSSize size;
	size.width = w;
	size.height = h;
	[_impl->win setContentSize:size];
}

////////////////////////////////////////

double window::scale_factor( void )
{
	return [_impl->win backingScaleFactor];
}

////////////////////////////////////////

void window::resize_event( double w, double h )
{
	_last_w = w;
	_last_h = h;
	if ( resized )
		resized( _last_w, _last_h );
}

////////////////////////////////////////

void window::invalidate( const base::rect & )
{
	[_impl->view performSelector:@selector(forceRedraw) withObject:nil afterDelay:0.0];
}

////////////////////////////////////////

void window::set_minimum_size( double /*w*/, double /*h*/ )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	NSString *tstring = [[NSString alloc] initWithUTF8String:t.c_str()];
	[_impl->win setTitle:tstring];
}

////////////////////////////////////////

void window::acquire( void )
{
	[[_impl->view openGLContext] makeCurrentContext];
}

////////////////////////////////////////

void window::release( void )
{
}

////////////////////////////////////////

void window::set_ns( void *w, void *v )
{
	_impl->win = static_cast<NSWindow *>( w );
	_impl->view = static_cast<NSOpenGLView *>( v );
	if ( [_impl->view respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:)] )
		[_impl->view setWantsBestResolutionOpenGLSurface:YES];

	double scale = scale_factor();
	NSSize size = [_impl->view bounds].size;
	_last_w = size.width * scale;
	_last_h = size.height * scale;
}

////////////////////////////////////////

} }

