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

#include <platform/platform.h>
#include <platform/menu.h>
#include <platform/tray.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <stdexcept>
#include <cstring>

////////////////////////////////////////

namespace
{

////////////////////////////////////////

static void
global_registry_handler( void *data,
						 struct wl_registry *registry,
						 uint32_t id,
						 const char *interface,
						 uint32_t version )
{
	std::cout << "wayland global_registry interface '" << interface << "' ID " << id << " (version " << version << ")" << std::endl;

	platform::wayland::system *sysPtr = reinterpret_cast<platform::wayland::system *>( data );
	if ( ! sysPtr )
		return;
	if ( 0 == strcmp( interface, "wl_compositor" ) )
	{
		sysPtr->set_compositor( (struct wl_compositor *)wl_registry_bind( registry, id, &wl_compositor_interface, version ) );
	}
	else if ( 0 == strcmp( interface, "wl_shell" ) )
	{
		sysPtr->set_shell( (struct wl_shell *)wl_registry_bind( registry, id, &wl_shell_interface, version ) );
	}
	// add seat and other input objects
}

////////////////////////////////////////

static void
global_registry_remover( void *data,
						 struct wl_registry *reg,
						 uint32_t id )
{
	std::cout << "wayland global_registry REMOVE ID " << id << std::endl;
	//system *sysPtr = reinterpret_cast<system *>( data );
	// remove input objects if they disappear
}

static const struct wl_registry_listener sys_reg_listener = 
{
	&global_registry_handler,
	&global_registry_remover
};

} // empty namespace

////////////////////////////////////////

namespace platform
{
namespace wayland
{

////////////////////////////////////////

system::system( const std::string &d )
		: ::platform::system( "wayland", "Wayland/EGL" )
{
	// TODO: dynamically load libEGL.so
	const char *dname = nullptr;
	if ( ! d.empty() )
		dname = d.c_str();

	_display.reset( wl_display_connect( dname ), []( struct wl_display *ptr ) { if ( ptr ) wl_display_disconnect( ptr ); } );
	if ( _display )
	{
		wl_display_set_user_data( _display.get(), this );

		// huh, thought this would have to be a member, but it seems
		// once a listener is added, you can blow away the registry
		// reference (until you might need it again)? although I guess
		// in the initial pass, you register bindings to the registry
		// and that increments the ref count
		std::shared_ptr<struct wl_registry> reg;
		reg.reset( wl_display_get_registry( _display.get() ), []( struct wl_registry *ptr ) { if ( ptr ) wl_registry_destroy( ptr ); } );
		if ( ! reg )
			throw std::runtime_error( "unable to create global wayland registry" );
		wl_registry_set_user_data( reg.get(), this );
		wl_registry_add_listener( reg.get(), &sys_reg_listener, this );

		// trigger a round trip so the compositor, etc. and other seats,
		// etc. are found
		wl_display_dispatch( _display.get() );
		wl_display_roundtrip( _display.get() );

		_egl_disp = eglGetDisplay( (EGLNativeDisplayType)_display.get() );
		EGLint maj = 0, min = 0;
		eglInitialize( _egl_disp, &maj, &min );
		if ( maj < 1 || ( maj == 1 && min < 4 ) )
			throw std::runtime_error( "EGL version too old, reuire 1.4 to support OpenGL API" );

		std::cout << "EGL version " << maj << '.' << min << std::endl;
		if ( ! eglBindAPI( EGL_OPENGL_API ) )
			throw std::runtime_error( "Error binding to OpenGL API" );

		// need to add / remove screens based on registry
		_screens.emplace_back( std::make_shared<screen>() );

		_dispatcher = std::make_shared<dispatcher>( this, _display );

		int err = gl3wInit2( eglGetProcAddress );
		if ( err != 0 )
			throw std::runtime_error( "failed to intialize EGL" );

	}
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

const std::shared_ptr<::platform::screen> &
system::default_screen( void )
{
	std::cerr << " TODO" << std::endl;
	return _screens.front();
}

////////////////////////////////////////

std::shared_ptr<::platform::cursor>
system::new_cursor( void )
{
	throw_not_yet();
}

////////////////////////////////////////

std::shared_ptr<::platform::cursor>
system::builtin_cursor( standard_cursor sc )
{
	throw_not_yet();
}

////////////////////////////////////////

void
system::set_selection( selection sel )
{
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_selection( selection_type sel,
						 const std::vector<std::string> &allowedMimeTypes,
						 const std::string &clipboardName )
{
	return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_selection( selection_type sel,
						 const selection_type_function &chooseMimeType,
						 const std::string &clipboardName )
{
	return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

const std::vector<std::string> &
system::default_string_types( void )
{
	static std::vector<std::string> xlibTypes{ "text/plain;charset=utf-8", "text/plain" };
	return xlibTypes;
}

////////////////////////////////////////

selection_type_function
system::default_string_selector( void )
{
	auto selFun = [](const std::vector<std::string> &l) -> std::string 
		{
			std::string ret{ "text/plain;charset=utf-8" };
			if ( std::find( l.begin(), l.end(), ret ) != l.end() )
				return ret;
			ret = "text/plain";
			if ( std::find( l.begin(), l.end(), ret ) != l.end() )
				return ret;
			return std::string();
		};
	return selection_type_function{ selFun };
}

////////////////////////////////////////

system::mime_converter
system::default_string_converter( void )
{
	auto convFun = [](const std::vector<uint8_t> &d, const std::string &cur, const std::string &to) -> std::vector<uint8_t> 
		{
			if ( base::begins_with( cur, "text/plain" ) )
			{
				if ( base::begins_with( to, "text/plain" ) )
					return d;
			}
			return std::vector<uint8_t>();
		};
	return mime_converter{ convFun };
}

////////////////////////////////////////

void
system::begin_drag( selection sel,
					const std::shared_ptr<::platform::cursor> &cursor )
{
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_drop( const selection_type_function &chooseMimeType )
{
	return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

std::shared_ptr<::platform::menu>
system::new_system_menu( void )
{
	return std::make_shared<::platform::menu>();
}

////////////////////////////////////////

std::shared_ptr<::platform::tray>
system::new_system_tray_item( void )
{
	return std::make_shared<::platform::tray>();
}

////////////////////////////////////////

std::shared_ptr<::platform::window> system::new_window( window_type wintype, const std::shared_ptr<::platform::screen> &s )
{
	auto ret = std::make_shared<window>( wintype, _egl_disp, _compositor, _shell, s );
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

bool system::is_available( void )
{
	struct wl_display *d = wl_display_connect( nullptr );
	bool result = ( d != nullptr );
	wl_display_disconnect( d );
	return result;
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform
