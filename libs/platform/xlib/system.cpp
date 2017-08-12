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
#include <gl/opengl.h>

#include <dlfcn.h>
#include <mutex>
#include <stdexcept>

#include <platform/platform.h>
#include <base/contract.h>
#include <base/env.h>

////////////////////////////////////////

namespace {

void *opengl_dso = nullptr;
platform::system::opengl_query glx_query = nullptr;
std::once_flag opengl_init_flag;

void shutdown_opengl( void )
{
	if ( opengl_dso )
		dlclose( opengl_dso );
	glx_query = nullptr;
}

void init_opengl( void )
{
	opengl_dso = dlopen( "libGL.so", RTLD_GLOBAL | RTLD_LAZY );
	if ( opengl_dso )
	{
		glx_query = (platform::system::opengl_query) dlsym( opengl_dso, "glXGetProcAddressARB" );
		atexit( shutdown_opengl );
	}
}

platform::system::opengl_func_ptr
queryGL( const char *fname )
{
	platform::system::opengl_func_ptr ret = nullptr;
	if ( glx_query )
		ret = glx_query( fname );
	if ( ! ret && opengl_dso )
		ret = (platform::system::opengl_func_ptr) dlsym( opengl_dso, fname );

	return ret;
}

int
xErrorCB( Display *d, XErrorEvent *e )
{
	char errorBuf[4096];

	XGetErrorText( d, e->error_code, errorBuf, 4096 );
	std::cerr << "ERROR: Xlib Error"
			  << "\n  Major/Minor: " << int(e->request_code) << " / " << int(e->minor_code)
			  << "\n   Error code: " << int(e->error_code)
			  << "\n      Message: " << errorBuf << std::endl;

	return 0;
}

int
xIOErrorCB( Display * )
{
	std::cerr << "ERROR: I/O error w/ X server (connection lost?)" << std::endl;
	exit( -1 );
}

void CloseDisplay( Display *disp )
{
	if ( disp != nullptr )
		XCloseDisplay( disp );
}


}


namespace platform { namespace xlib
{

////////////////////////////////////////

system::system( const std::string &d )
		: ::platform::system( "x11", "X11/XLib" )
{
	std::call_once( opengl_init_flag, [](){ init_opengl(); } );

	const char *dname = nullptr;
	if ( ! d.empty() )
		dname = d.c_str();
	std::string disenv = base::env::global().get( "DISPLAY" );
	if ( ! dname && ! disenv.empty() )
		dname = disenv.c_str();

	XSetErrorHandler( &xErrorCB );
	XSetIOErrorHandler( &xIOErrorCB );

	_display.reset( XOpenDisplay( dname ), &CloseDisplay );
	if (  !_display )
		return;

	if ( ! XSupportsLocale() )
		throw_runtime( "Current locale not supported by X" );

	if ( XSetLocaleModifiers( "@im=none" ) == nullptr )
		throw_runtime( "Unable to set locale modifiers for Xlib" );

	_screens.resize( static_cast<size_t>( ScreenCount( _display.get() ) ) );
	for ( int i = 0; i < ScreenCount( _display.get() ); ++i )
		_screens[0] = std::make_shared<screen>( _display, i );

	// don't have a good way to identify individual keyboards
	// in raw xlib?
	_keyboard = std::make_shared<keyboard>();
	// don't have a good way to identify individual keyboards
	// in raw xlib?
	_mouse = std::make_shared<mouse>();

	_dispatcher = std::make_shared<dispatcher>( _display, _keyboard, _mouse );
//	_dispatcher->add_waitable( _keyboard );
//	_dispatcher->add_waitable( _mouse );

	if ( ! gl3wInit2( queryGL ) )
		throw_runtime( "Unable to initialize OpenGL" );
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

system::opengl_query
system::gl_proc_address( void )
{
	return queryGL;
}

////////////////////////////////////////

std::shared_ptr<::platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>( *this, _display );
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

std::shared_ptr<::platform::dispatcher> system::get_dispatcher( void )
{
	return _dispatcher;
}

////////////////////////////////////////

std::shared_ptr<::platform::keyboard> system::get_keyboard( void )
{
	return _keyboard;
}

////////////////////////////////////////

std::shared_ptr<::platform::mouse> system::get_mouse( void )
{
	return _mouse;
}

////////////////////////////////////////

} }
