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
#include <gl/gl3w.h>

#include <mutex>
#include <stdexcept>
#include <dlfcn.h>

#include <base/contract.h>

#include <Cocoa/Cocoa.h>

namespace
{
void *libgl = nullptr;
std::once_flag opengl_init_flag;

void shutdown_libgl(void)
{
	if ( libgl )
		dlclose( libgl );
}

void init_libgl(void)
{
	libgl = dlopen( "/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_GLOBAL | RTLD_LAZY );
	atexit( shutdown_libgl );
}

platform::system::opengl_func_ptr
queryGL( const char *f )
{
	if ( libgl )
		return (platform::system::opengl_func_ptr) dlsym( libgl, f );

	return nullptr;
}

} // empty namespace

namespace platform { namespace cocoa
{

////////////////////////////////////////

system::system( const std::string & )
	: platform::system( "cocoa", "Cocoa" )
{
	std::call_once( opengl_init_flag, [](){ init_libgl(); } );

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

	if ( ! gl3wInit2( queryGL ) )
		throw_runtime( "Unable to initialize OpenGL" );
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

bool
system::is_working( void ) const
{
	return true;
}

////////////////////////////////////////

system::opengl_query
system::gl_proc_address( void )
{
	return queryGL;
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

std::shared_ptr<menu> system::new_system_menu( void )
{
	return std::shared_ptr<menu>();
}

////////////////////////////////////////

std::shared_ptr<tray> system::new_system_tray_item( void )
{
	return std::shared_ptr<tray>();
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<::platform::cocoa::window>();
	_dispatcher->add_window( ret );
	return ret;
}

////////////////////////////////////////

void
system::destroy_window( const std::shared_ptr<::platform::window> &w )
{
	auto x = std::static_pointer_cast<window>( w );
	_dispatcher->remove_window( x );
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

} // namespace cocoa
} // namespace platform

