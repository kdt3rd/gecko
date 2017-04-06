//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "system.h"
#include "screen.h"
#include "window.h"
#include "dispatcher.h"

#include <base/contract.h>
#include <stdexcept>

#include <Cocoa/Cocoa.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

system::system( void )
	: platform::system( "cocoa", "Cocoa" )
{
    [NSAutoreleasePool new];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSMenu *menubar = [[NSMenu new] autorelease];
    NSMenuItem *appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    NSMenu *appMenu = [[NSMenu new] autorelease];
    NSString *appName = [[NSProcessInfo processInfo] processName];
    NSString *quitTitle = [@"Quit " stringByAppendingString:appName];
    NSMenuItem *quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"] autorelease];

    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];

	_keyboard = std::make_shared<keyboard>();
	_mouse = std::make_shared<mouse>();
	_dispatcher = std::make_shared<dispatcher>( _keyboard, _mouse );

    [NSApp activateIgnoringOtherApps:YES];
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

std::vector<std::shared_ptr<::platform::screen>> system::screens( void )
{
	std::vector<std::shared_ptr<::platform::screen>> result;
	NSArray *s = [NSScreen screens];
	for ( size_t i = 0; i < [s count]; ++i )
	{
		auto scr = std::make_shared<screen>( [s objectAtIndex:i] );
		result.push_back( scr );
	}

	return result;
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<::platform::cocoa::window>();
	_dispatcher->add_window( ret );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<platform::dispatcher> system::get_dispatcher( void )
{
	return _dispatcher;
}

////////////////////////////////////////

std::shared_ptr<platform::keyboard> system::get_keyboard( void )
{
	return _keyboard;
}

////////////////////////////////////////

std::shared_ptr<platform::mouse> system::get_mouse( void )
{
	return _mouse;
}

////////////////////////////////////////

} }

