
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include <core/pointer.h>
#include "dispatcher.h"
#include <Cocoa.h>

////////////////////////////////////////

@interface MyView : NSView
{
	std::shared_ptr<cocoa::window> win;
	std::shared_ptr<cocoa::mouse> mouse;
}
@end

////////////////////////////////////////

@implementation MyView

////////////////////////////////////////

- (BOOL)isFlipped
{
	return YES;
}

////////////////////////////////////////

- (id)initWithWindow:(std::shared_ptr<cocoa::window>)w andMouse:(std::shared_ptr<cocoa::mouse>)m
{
	self = [super init];
	if ( self )
	{
		win = w;
		mouse = m;
	}

	return self;
}

////////////////////////////////////////

- (void)drawRect:(NSRect)rect
{
	try
	{
		win->exposed();
	}
	catch ( std::exception &e )
	{
		std::cout << "Ooops: " << e.what() << std::endl;
	}
}

////////////////////////////////////////

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	try
	{
		win->resized( newSize.width, newSize.height );
	}
	catch ( std::exception &e )
	{
		std::cout << "Ooops: " << e.what() << std::endl;
	}
}

////////////////////////////////////////

- (void)mouseDown:(NSEvent*)event
{
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_pressed( mouse, { p.x, p.y }, 1 );
}

////////////////////////////////////////

- (void)mouseUp:(NSEvent*)event
{
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_released( mouse, { p.x, p.y }, 1 );
}

////////////////////////////////////////

- (void)mouseDragged:(NSEvent*)event
{
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_moved( mouse, { p.x, p.y } );
}

////////////////////////////////////////

@end


namespace cocoa
{

////////////////////////////////////////

dispatcher::dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _keyboard( k ), _mouse( m )
{
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;
    [NSApp run];

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
	[NSApp stop:nil];
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	int style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

	NSWindow *nswin = [[[NSWindow alloc] initWithContentRect:NSMakeRect( 0, 0, 200, 200 )
		styleMask:style backing:NSBackingStoreBuffered defer:YES]
			autorelease];

	[nswin orderOut:nil];
	[nswin cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[nswin setIgnoresMouseEvents:NO];

	MyView *view = [[MyView alloc] initWithWindow:w andMouse:_mouse];
	[nswin setContentView:view];
	[nswin setInitialFirstResponder:view];

	w->set_ns( nswin, view );
}

////////////////////////////////////////

}

