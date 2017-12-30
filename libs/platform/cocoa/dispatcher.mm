//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include <base/pointer.h>
#include "dispatcher.h"
#include <Cocoa/Cocoa.h>

////////////////////////////////////////

@interface MyView : NSOpenGLView
{
}
@end

////////////////////////////////////////

@implementation MyView
{
	std::shared_ptr<::platform::cocoa::window> _win;
	std::shared_ptr<::platform::cocoa::mouse> _mouse;
	std::shared_ptr<::platform::cocoa::keyboard> _keyboard;
}

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::window> &)win
{ return self->_win; }

- (void)setWin: (const std::shared_ptr<::platform::cocoa::window> &)w
{ self->_win = w; }

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::mouse> &)mouse
{ return _mouse; }

- (void)setMouse: (const std::shared_ptr<::platform::cocoa::mouse> &)m
{ _mouse = m; }

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::keyboard> &)keyboard
{ return _keyboard; }

- (void)setKeyboard: (const std::shared_ptr<::platform::cocoa::keyboard> &)k
{ _keyboard = k; }

////////////////////////////////////////

- (BOOL)isFlipped
{
	return YES;
}

////////////////////////////////////////

- (void)reshape
{
	[super setNeedsDisplay: YES];
}

////////////////////////////////////////

- (void)forceRedraw
{
	[super setNeedsDisplay: YES];
}

////////////////////////////////////////

- (id)initWithWindow:(std::shared_ptr<::platform::cocoa::window>)w andMouse:(std::shared_ptr<::platform::cocoa::mouse>)m andKeyboard: (std::shared_ptr<::platform::cocoa::keyboard>)k
{
	[[self superview] setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
	self = [super init];
	if ( self )
	{
		[self setWin: w];
		[self setMouse: m];
		[self setKeyboard: k];
	}

	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
	{
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAMultisample,
		NSOpenGLPFASampleBuffers, static_cast<NSOpenGLPixelFormatAttribute>(1),
		NSOpenGLPFASamples, static_cast<NSOpenGLPixelFormatAttribute>(4),
		0
	};

	NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
	NSOpenGLContext *openGLContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] autorelease];
	if ( openGLContext == nullptr )
		throw std::runtime_error( "no opengl context" );
	[self setOpenGLContext:openGLContext];
	[openGLContext makeCurrentContext]; 

	return self;
}

////////////////////////////////////////

- (void)drawRect:(NSRect)rect
{
	if ( [self win]->exposed )
		[self win]->exposed();
	[[self openGLContext] flushBuffer];
}

////////////////////////////////////////

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	double scale = [self win]->scale_factor();
	[self win]->resize_event( static_cast<platform::coord_type>( newSize.width * scale ),
							  static_cast<platform::coord_type>( newSize.height * scale ) );
}

////////////////////////////////////////

- (BOOL)acceptsFirstResponder
{
	return YES;
}

////////////////////////////////////////

- (void)mouseDown:(NSEvent*)event
{
	double scale = [self win]->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	if ( [self win]->mouse_pressed )
		[self win]->mouse_pressed(
			*[self mouse],
			{ static_cast<platform::coord_type>( p.x * scale ),
					static_cast<platform::coord_type>( p.y * scale ) }, 1 );
}

////////////////////////////////////////

- (void)mouseUp:(NSEvent*)event
{
	double scale = [self win]->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	if ( [self win]->mouse_released )
		[self win]->mouse_released(
			*[self mouse],
			{ static_cast<platform::coord_type>( p.x * scale ),
					static_cast<platform::coord_type>( p.y * scale ) }, 1 );
}

////////////////////////////////////////

- (void)mouseDragged:(NSEvent*)event
{
	double scale = [self win]->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	if ( [self win]->mouse_moved )
		[self win]->mouse_moved(
			*[self mouse],
			{ static_cast<platform::coord_type>( p.x * scale ),
					static_cast<platform::coord_type>( p.y * scale ) } );
}

////////////////////////////////////////

- (void)keyDown:(NSEvent*)event
{
	unsigned short kc = [event keyCode];
	platform::scancode sc = [self keyboard]->get_scancode( kc );

	if ( [self win]->key_pressed )
		[self win]->key_pressed( *[self keyboard], sc );

	NSString *chars = [event characters];
	char32_t c;
	if ( [chars getBytes:&c maxLength:4 usedLength:NULL encoding:NSUTF32LittleEndianStringEncoding options:0 range:NSMakeRange(0,1) remainingRange:NULL] )
	{
		c = NSSwapLittleIntToHost( c );
		if ( c < 0xE000 || c > 0xF8FF ) // Private area
		{
			if ( [self win]->text_entered )
				[self win]->text_entered( *[self keyboard], c );
		}
	}
}

////////////////////////////////////////

- (void)keyUp:(NSEvent*)event
{
	unsigned short kc = [event keyCode];
	platform::scancode sc = [self keyboard]->get_scancode( kc );
	if ( [self win]->key_released )
		[self win]->key_released( *[self keyboard], sc );
}

////////////////////////////////////////

@end


namespace platform { namespace cocoa
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

/*
	[NSApp finishLaunching];

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	_continue_running = true;

	do
	{
		[pool release];
		pool = [[NSAutoreleasePool alloc] init];

		NSEvent *event = [NSApp
			nextEventMatchingMask:NSAnyEventMask
			untilDate:[NSDate distantFuture]
			inMode:NSDefaultRunLoopMode
			dequeue:YES];

std::cout << "Sending event" << std::endl;
		[NSApp sendEvent:event];
std::cout << "Done event" << std::endl;
		[NSApp updateWindows];
	} while ( _continue_running );

	[pool release];
*/
	std::cout << "TODO: Add waitable loop" << std::endl;
    [NSApp run];
	std::cout << "DONE EXECUTE" << std::endl;

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
//	_continue_running = false;
	[NSApp stop:nil];
}

////////////////////////////////////////

void dispatcher::add_waitable( const std::shared_ptr<waitable> &w )
{
	throw_not_yet();
}

////////////////////////////////////////

void dispatcher::remove_waitable( const std::shared_ptr<waitable> &w )
{
	throw_not_yet();
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	int style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

	NSWindow *nswin = [[[NSWindow alloc] initWithContentRect:NSMakeRect( 0, 0, 200, 200 )
		styleMask:style backing:NSBackingStoreBuffered defer:YES] autorelease];

	[nswin orderOut:nil];
	[nswin cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[nswin setIgnoresMouseEvents:NO];

	MyView *view = [[MyView alloc] initWithWindow:w andMouse:_mouse andKeyboard:_keyboard];
	[nswin setContentView:view];
	[nswin setInitialFirstResponder:view];

	w->set_ns( nswin, view );
}

////////////////////////////////////////

void
dispatcher::remove_window( const std::shared_ptr<window> &w )
{
}

////////////////////////////////////////

} }

