
#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include <base/pointer.h>
#include "dispatcher.h"
#include <Cocoa/Cocoa.h>

////////////////////////////////////////

@interface MyView : NSOpenGLView
{
	std::shared_ptr<::platform::cocoa::window> win;
	std::shared_ptr<::platform::cocoa::mouse> mouse;
	std::shared_ptr<::platform::cocoa::keyboard> keyboard;
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

- (void)reshape
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
		win = w;
		mouse = m;
		keyboard = k;
	}

	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
	{
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAMultisample,
		NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
		NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
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
	try
	{
		win->exposed();
		[[self openGLContext] flushBuffer];
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
		float scale = win->scale_factor();
		win->resize_event( newSize.width * scale, newSize.height * scale );
	}
	catch ( std::exception &e )
	{
		std::cout << "Ooops: " << e.what() << std::endl;
	}
}

////////////////////////////////////////

- (BOOL)acceptsFirstResponder
{
	return YES;
}

////////////////////////////////////////

- (void)mouseDown:(NSEvent*)event
{
	double scale = win->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_pressed( mouse, { p.x * scale, p.y * scale }, 1 );
}

////////////////////////////////////////

- (void)mouseUp:(NSEvent*)event
{
	double scale = win->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_released( mouse, { p.x * scale, p.y * scale }, 1 );
}

////////////////////////////////////////

- (void)mouseDragged:(NSEvent*)event
{
	double scale = win->scale_factor();
	NSPoint p = [event locationInWindow];
	p = [self convertPoint:p fromView:nil];
	win->mouse_moved( mouse, { p.x * scale, p.y * scale } );
}

////////////////////////////////////////

- (void)keyDown:(NSEvent*)event
{
	unsigned short kc = [event keyCode];
	platform::scancode sc = keyboard->get_scancode( kc );

	win->key_pressed( keyboard, sc );

	NSString *chars = [event characters];
	char32_t c;
	if ( [chars getBytes:&c maxLength:4 usedLength:NULL encoding:NSUTF32LittleEndianStringEncoding options:0 range:NSMakeRange(0,1) remainingRange:NULL] )
	{
		c = NSSwapLittleIntToHost( c );
		if ( c < 0xE000 || c > 0xF8FF ) // Private area
			win->text_entered( keyboard, c );
	}
}

////////////////////////////////////////

- (void)keyUp:(NSEvent*)event
{
	unsigned short kc = [event keyCode];
	platform::scancode sc = keyboard->get_scancode( kc );
	win->key_released( keyboard, sc );
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

} }

