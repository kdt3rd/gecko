
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
	std::cout << "Drawing view of " << (void *)win.get() << std::endl;
	CGContextRef ctxt = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	std::cout << "Context: " << (void*)ctxt << std::endl;
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
	std::cout << "Resized: " << newSize.width << 'x' << newSize.height << std::endl;
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
	std::cout << "Size: " << self.frame.size.height << ' ' << self.bounds.size.height << std::endl;
	std::cout << "Mouse down! " << p.x << ' ' << self.frame.size.width - p.y << std::endl;
	win->mouse_pressed( mouse, { p.x, self.frame.size.width - p.y }, 1 );
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

	w->set_ns_window( nswin );

	[nswin orderOut:nil];
	[nswin cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[nswin setIgnoresMouseEvents:NO];

	MyView *view = [[MyView alloc] initWithWindow:w andMouse:_mouse];
	[nswin setContentView:view];
	[nswin setInitialFirstResponder:view];
}

////////////////////////////////////////

}

