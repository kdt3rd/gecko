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

#include <mutex>

#include <base/contract.h>

namespace
{

HMODULE libgl = NULL;
std::once_flag opengl_init_flag;

void shutdown_libgl( void )
{
	if ( libgl )
		FreeLibrary( libgl );
	libgl = NULL;
}

void init_libgl( void )
{
	libgl = LoadLibraryA( "opengl32.dll" );
	atexit( shutdown_libgl );
}

platform::system::opengl_func_ptr
queryGL( const char *f )
{
	platform::system::opengl_func_ptr res;

	res = (platform::system::opengl_func_ptr) wglGetProcAddress( f );
	if ( ! res && libgl )
		res = (platform::system::opengl_func_ptr) GetProcAddress( libgl, f );
	return res;
}

BOOL CALLBACK monitorEnumCB( HMONITOR hMon,
							 HDC hdcMon,
							 LPRECT lprcMonitor,
							 LPARAM userdata )
{
	std::vector<std::shared_ptr<::platform::screen>> *sl =
		reinterpret_cast<std::vector<std::shared_ptr<::platform::screen>> *>( userdata );
	sl->emplace_back( std::make_shared<platform::mswin::screen>( hMon ) );
}

}

namespace platform { namespace mswin
{

////////////////////////////////////////

system::system( const std::string & )
	: platform::system( "mswin", "Microsoft Windows" )
{
	std::call_once( opengl_init_flag, [](){ init_libgl(); } );

	_keyboard = std::make_shared<keyboard>();
	_mouse = std::make_shared<mouse>();
	_dispatcher = std::make_shared<dispatcher>( _keyboard, _mouse );

	EnumDisplayMonitors( NULL, NULL, monitorEnumCB, (LPARAM)&_screens);

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

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>();
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

} }

