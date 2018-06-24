//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"

#include <base/pointer.h>
#include <base/contract.h>
#include <stdexcept>

#include "context.h"
#include <platform/screen.h>

#include <Cocoa/Cocoa.h>

namespace platform { namespace cocoa
{

struct window::objcwrapper
{
	NSWindow *win = nullptr;
	NSOpenGLView *view = nullptr;
	CGContext *ctxt = nullptr;
};

////////////////////////////////////////

window::window( const std::shared_ptr<::platform::screen> &s, const rect &p )
	: ::platform::window( s, p ),
	  _impl( new objcwrapper ),
	  _context( std::make_shared<context>() )
{
	int style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

	// window coordinates are flipped - 0 is at lower left
	rect scrbounds = s->bounds( true );

	auto y = scrbounds.height() - p.height() - p.y();
	NSWindow *nswin = [[[NSWindow alloc] initWithContentRect:NSMakeRect( p.x(), y, p.width(), p.height() )
		styleMask:style backing:NSBackingStoreBuffered defer:YES] autorelease];
	_impl->win = nswin;

	[nswin orderOut:nil];
	[nswin cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[nswin setIgnoresMouseEvents:NO];
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

::platform::context &
window::hw_context( void )
{
	return *_context;
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
	NSUInteger mask = NSWindowStyleMaskBorderless;
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
	[_impl->win orderOut:nil];
}

////////////////////////////////////////

bool window::is_visible( void )
{
	// TODO fix this
	return true;
}

////////////////////////////////////////

void window::fullscreen( bool fs )
{
	// TODO:
}

////////////////////////////////////////

double window::scale_factor( void )
{
	return [_impl->win backingScaleFactor];
}

////////////////////////////////////////

void window::apply_minimum_size( coord_type w, coord_type h )
{
//	NSSize size;
//	size.width = w;
//	size.height = h;
//	[_impl->win setMinSize:size];
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	NSString *tstring = [[NSString alloc] initWithUTF8String:t.c_str()];
	[_impl->win setTitle:tstring];
}

////////////////////////////////////////

void window::set_ns( void *v )
{
	NSOpenGLView *view = static_cast<NSOpenGLView *>( v );
	[_impl->win setContentView:view];
	[_impl->win setInitialFirstResponder:view];

	_context->set_ns( _impl->win, v );
	_impl->view = view;
	if ( [view respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:)] )
		[view setWantsBestResolutionOpenGLSurface:YES];

//	double scale = scale_factor();
//	NSSize size = [_impl->view bounds].size;
//	_last_w = size.width * scale;
//	_last_h = size.height * scale;
}

////////////////////////////////////////

void window::make_current( const std::shared_ptr<cursor> & )
{
	// TODO
}

////////////////////////////////////////

rect window::query_geometry( void )
{
	NSRect cgr = [_impl->win frame];

	cgr = [_impl->win contentRectForFrameRect:cgr];

	rect scrbounds = query_screen()->bounds( true );
	auto y = scrbounds.height() - ( cgr.origin.y + cgr.size.height );

	// this includes the title bar?
	return rect( cgr.origin.x, cgr.origin.y, cgr.size.width, cgr.size.height );
}

////////////////////////////////////////

bool
window::update_geometry( rect &r )
{
	NSRect cgr;
	cgr.origin.x = r.x();
	cgr.origin.y = r.y();
	cgr.size.width = r.width();
	cgr.size.height = r.height();

	rect scrbounds = query_screen()->bounds( true );
	auto y = scrbounds.height() - r.height() - r.y();

	cgr = [_impl->win frameRectForContentRect:cgr];

	[_impl->win setFrame:cgr display:true];

	r = query_geometry();
	return true;
}

////////////////////////////////////////

void window::submit_delayed_expose( const rect &r )
{
	//[_impl->view performSelector:@selector(forceRedraw) withObject:nil afterDelay:0.0];
	[_impl->view setNeedsDisplay:YES];
}

////////////////////////////////////////

} // namespace cocoa
} // namespace platform

