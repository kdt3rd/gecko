// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "dispatcher.h"

#include "window.h"

#include <Cocoa/Cocoa.h>
#include <base/contract.h>
#include <base/pointer.h>
#include <cstdlib>
#include <iostream>
#include <platform/event.h>
#include <platform/event_queue.h>

////////////////////////////////////////

@interface MyView : NSOpenGLView
{}
@end

////////////////////////////////////////

@implementation MyView
{
    std::shared_ptr<::platform::cocoa::window>   _win;
    std::shared_ptr<::platform::cocoa::mouse>    _mouse;
    std::shared_ptr<::platform::cocoa::keyboard> _keyboard;
    ::platform::event_source *                   _def_evtsource;
}

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::window> &)win
{
    return self->_win;
}

- (void)setWin:(const std::shared_ptr<::platform::cocoa::window> &)w
{
    self->_win = w;
}

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::mouse> &)mouse
{
    return self->_mouse;
}

- (void)setMouse:(const std::shared_ptr<::platform::cocoa::mouse> &)m
{
    self->_mouse = m;
}

////////////////////////////////////////

- (const std::shared_ptr<::platform::cocoa::keyboard> &)keyboard
{
    return self->_keyboard;
}

- (void)setKeyboard:(const std::shared_ptr<::platform::cocoa::keyboard> &)k
{
    self->_keyboard = k;
}

////////////////////////////////////////

- ( ::platform::event_source *)source
{
    return self->_def_evtsource;
}
- (void)setEventQueue:( ::platform::event_source *)s
{
    self->_def_evtsource = s;
}

////////////////////////////////////////

- (BOOL)isFlipped
{
    return YES;
}

////////////////////////////////////////

- (void)reshape
{
    [super setNeedsDisplay:YES];
}

////////////////////////////////////////

- (void)forceRedraw
{
    [super setNeedsDisplay:YES];
}

////////////////////////////////////////

- (id)initWithWindow:(std::shared_ptr<::platform::cocoa::window>)w
            andMouse:(std::shared_ptr<::platform::cocoa::mouse>)m
         andKeyboard:(std::shared_ptr<::platform::cocoa::keyboard>)k
           andSource:( ::platform::event_queue *)es
{
    [[self superview]
        setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    self = [super init];
    if ( self )
    {
        [self setWin:w];
        [self setMouse:m];
        [self setKeyboard:k];
        [self setEventQueue:es];
    }

    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAMultisample,
        NSOpenGLPFASampleBuffers,
        static_cast<NSOpenGLPixelFormatAttribute>( 1 ),
        NSOpenGLPFASamples,
        static_cast<NSOpenGLPixelFormatAttribute>( 4 ),
        0
    };

    NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]
        initWithAttributes:pixelFormatAttributes] autorelease];
    NSOpenGLContext *    openGLContext =
        [[[NSOpenGLContext alloc] initWithFormat:pixelFormat
                                    shareContext:nil] autorelease];
    if ( openGLContext == nullptr )
        throw std::runtime_error( "no opengl context" );
    [self setOpenGLContext:openGLContext];
    [openGLContext makeCurrentContext];

    return self;
}

////////////////////////////////////////

- (void)drawRect:(NSRect)rect
{
    using namespace platform;

    [self win] -> process_event( event::window(
                   [self source], event_type::WINDOW_EXPOSED, 0, 0, 0, 0 ) );
}

////////////////////////////////////////

- (void)setFrameSize:(NSSize)newSize
{
    using namespace platform;
    [super setFrameSize:newSize];
    double scale = [self win] -> scale_factor();
    [self win] -> process_event( event::window(
                   [self source],
                   event_type::WINDOW_RESIZED,
                   0,
                   0,
                   static_cast<coord_type>( newSize.width * scale ),
                   static_cast<coord_type>( newSize.height * scale ) ) );
}

////////////////////////////////////////

- (BOOL)acceptsFirstResponder
{
    return YES;
}

////////////////////////////////////////

- (void)mouseDown:(NSEvent *)event
{
    using namespace platform;

    double  scale = [self win] -> scale_factor();
    NSPoint p     = [event locationInWindow];
    p             = [self convertPoint:p fromView:nil];
    // TODO: fill in modifiers and system
    [self win] -> process_event( event::mouse(
                   [self mouse].get(),
                   event_type::MOUSE_DOWN,
                   static_cast<platform::coord_type>( p.x * scale ),
                   static_cast<platform::coord_type>( p.y * scale ),
                   1,
                   0 ) );
}

////////////////////////////////////////

- (void)mouseUp:(NSEvent *)event
{
    using namespace platform;

    double  scale = [self win] -> scale_factor();
    NSPoint p     = [event locationInWindow];
    p             = [self convertPoint:p fromView:nil];
    // TODO: fill in modifiers and system
    [self win] -> process_event( event::mouse(
                   [self mouse].get(),
                   event_type::MOUSE_UP,
                   static_cast<platform::coord_type>( p.x * scale ),
                   static_cast<platform::coord_type>( p.y * scale ),
                   1,
                   0 ) );
}

////////////////////////////////////////

- (void)mouseDragged:(NSEvent *)event
{
    using namespace platform;

    double  scale = [self win] -> scale_factor();
    NSPoint p     = [event locationInWindow];
    p             = [self convertPoint:p fromView:nil];
    [self win] -> process_event( event::mouse(
                   [self mouse].get(),
                   event_type::MOUSE_MOVE,
                   static_cast<platform::coord_type>( p.x * scale ),
                   static_cast<platform::coord_type>( p.y * scale ),
                   0,
                   0 ) );
}

////////////////////////////////////////

- (void)keyDown:(NSEvent *)event
{
    using namespace platform;

    unsigned short kc = [event keyCode];
    scancode       sc = [self keyboard] -> get_scancode( kc );

    // TODO: query mouse position (or extract from ns event)
    [self win]
        -> process_event( event::key(
            [self keyboard].get(), event_type::KEYBOARD_DOWN, 0, 0, sc, 0 ) );

    NSString *chars = [event characters];
    char32_t  c;
    if ( [chars getBytes:&c
                  maxLength:4
                 usedLength:NULL
                   encoding:NSUTF32LittleEndianStringEncoding
                    options:0
                      range:NSMakeRange( 0, 1 )
             remainingRange:NULL] )
    {
        c = NSSwapLittleIntToHost( c );
        if ( c < 0xE000 || c > 0xF8FF ) // Private area
        {
            [self win] -> process_event( event::text(
                           [self keyboard].get(),
                           event_type::TEXT_ENTERED,
                           0,
                           0,
                           c,
                           0 ) );
        }
    }
}

////////////////////////////////////////

- (void)keyUp:(NSEvent *)event
{
    using namespace platform;

    unsigned short kc = [event keyCode];
    scancode       sc = [self keyboard] -> get_scancode( kc );
    // TODO: query mouse position (or extract from ns event)
    [self win]
        -> process_event( event::key(
            [self keyboard].get(), event_type::KEYBOARD_UP, 0, 0, sc, 0 ) );
}

////////////////////////////////////////

@end

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

dispatcher::dispatcher( ::platform::system *s )
    : ::platform::dispatcher( s )
    , _keyboard( std::make_shared<keyboard>( s ) )
    , _mouse( std::make_shared<mouse>( s ) )
{
    _event_source = CGEventSourceCreate( kCGEventSourceStateHIDSystemState );
    if ( !_event_source )
        throw_runtime( "Unable to initialize HID event source" );
    CGEventSourceSetLocalEventsSuppressionInterval( _event_source, 0.0 );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
    if ( _event_source )
        CFRelease( _event_source );
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
    MyView *view = [[MyView alloc] initWithWindow:w
                                         andMouse:_mouse
                                      andKeyboard:_keyboard
                                        andSource:_ext_events.get()];
    w->set_ns( view );
}

////////////////////////////////////////

void dispatcher::remove_window( const std::shared_ptr<window> &w ) {}

////////////////////////////////////////

}
}
