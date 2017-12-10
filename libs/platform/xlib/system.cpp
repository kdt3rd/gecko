//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "system.h"
#include "screen.h"
#include "window.h"
#include "cursor.h"
#include "dispatcher.h"
#include <gl/opengl.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/cursorfont.h>

#include <dlfcn.h>
#include <mutex>
#include <stdexcept>

#include <platform/platform.h>
#include <platform/scancode.h>
#include <platform/menu.h>
#include <platform/tray.h>
#include <base/contract.h>
#include <base/string_util.h>
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
	// TODO: do we want this or not????
	//XInitThreads();

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

	// TODO: look at using libinput even for xlib?
	// don't have a good way to identify individual keyboards / mice
	// in raw xlib? maybe it doesn't matter...
	_keyboard = std::make_shared<keyboard>();
	_mouse = std::make_shared<mouse>();

	_dispatcher = std::make_shared<dispatcher>( this, _display, _keyboard, _mouse );
	// keyboard and mouse are event sources, but we don't wait on
	// their input under xlib - the events are delivered to the dispatcher...
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

std::shared_ptr<::platform::cursor>
system::new_cursor( void )
{
	throw_not_yet();
}

////////////////////////////////////////

std::shared_ptr<::platform::cursor>
system::builtin_cursor( standard_cursor sc )
{
	auto lc = _cursors.find( sc );
	if ( lc == _cursors.end() )
	{
//		std::cout << "Current cursor theme: '" << XcursorGetTheme( _display.get() ) << "'" << std::endl;
		const char *cname = nullptr;
		unsigned int cchar = 0;
		switch ( sc )
		{
			case standard_cursor::DEFAULT: cname = "default"; cchar = XC_X_cursor; break;
			case standard_cursor::TEXT: cname = "text"; cchar = XC_xterm; break;
			case standard_cursor::URL_LINK_POINTER: cname = "pointer"; cchar = XC_hand1; break;
			case standard_cursor::HELP: cname = "help"; cchar = XC_question_arrow; break;
			case standard_cursor::CONTEXT_MENU: cname = "context-menu"; cchar = XC_arrow; break;
			case standard_cursor::PROGRESS: cname = "progress"; cchar = XC_coffee_mug; break;
			case standard_cursor::WAIT: cname = "wait"; cchar = XC_watch; break;
			case standard_cursor::DND_COPY: cname = "copy"; cchar = XC_arrow; break;
			case standard_cursor::DND_ALIAS: cname = "alias"; cchar = XC_arrow; break;
			case standard_cursor::DND_NO_DROP: cname = "no-drop"; cchar = XC_arrow; break;
			case standard_cursor::NOT_ALLOWED: cname = "not-allowed"; cchar = XC_pirate; break;
			case standard_cursor::ALL_SCROLL: cname = "all-scroll"; cchar = XC_arrow; break;
			case standard_cursor::RESIZE_ROW: cname = "row-resize"; cchar = XC_arrow; break;
			case standard_cursor::RESIZE_COL: cname = "col-resize"; cchar = XC_arrow; break;
			case standard_cursor::RESIZE_EAST: cname = "e-resize"; cchar = XC_right_side; break;
			case standard_cursor::RESIZE_NORTH_EAST: cname = "ne-resize"; cchar = XC_top_right_corner; break;
			case standard_cursor::RESIZE_NORTH: cname = "n-resize"; cchar = XC_top_side; break;
			case standard_cursor::RESIZE_NORTH_WEST: cname = "nw-resize"; cchar = XC_top_left_corner; break;
			case standard_cursor::RESIZE_WEST: cname = "w-resize"; cchar = XC_left_side; break;
			case standard_cursor::RESIZE_SOUTH_WEST: cname = "sw-resize"; cchar = XC_bottom_left_corner; break;
			case standard_cursor::RESIZE_SOUTH: cname = "s-resize"; cchar = XC_bottom_side; break;
			case standard_cursor::RESIZE_SOUTH_EAST: cname = "se-resize"; cchar = XC_bottom_right_corner; break;
			case standard_cursor::RESIZE_EAST_WEST: cname = "ew-resize"; cchar = XC_sb_h_double_arrow; break;
			case standard_cursor::RESIZE_NORTH_SOUTH: cname = "ns-resize"; cchar = XC_sb_v_double_arrow; break;
			case standard_cursor::RESIZE_NORTH_EAST_SOUTH_WEST: cname = "nesw-resize"; cchar = XC_arrow; break;
			case standard_cursor::RESIZE_NORTH_WEST_SOUTH_EAST: cname = "nwse-resize"; cchar = XC_arrow; break;
			case standard_cursor::VERTICAL_TEXT: cname = "vertical-text"; cchar = XC_arrow; break;
			case standard_cursor::CROSSHAIR: cname = "crosshair"; cchar = XC_crosshair; break;
			case standard_cursor::CELL: cname = "cell"; cchar = XC_arrow; break;

			default: cname = "default"; cchar = XC_arrow; break;
		}

		Cursor c = XcursorLibraryLoadCursor( _display.get(), cname );
		if ( c == None )
			c = XCreateFontCursor( _display.get(), cchar );

		auto r = std::make_shared<::platform::xlib::cursor>( c );
		_cursors[sc] = r;
		return r;
	}
	return lc->second;
}

////////////////////////////////////////

void
system::set_selection( selection sel )
{
	_dispatcher->set_selection( std::move( sel ) );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_selection( selection_type sel,
						 const std::vector<std::string> &allowedMimeTypes,
						 const std::string &clipboardName )
{
	return _dispatcher->query_selection( sel, allowedMimeTypes, clipboardName );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_selection( selection_type sel,
						 const selection_type_function &chooseMimeType,
						 const std::string &clipboardName )
{
	return _dispatcher->query_selection( sel, chooseMimeType, clipboardName );
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

				if ( to == "UTF8_STRING" || to == "STRING" || to == "STRING" ||
					 to == "C_STRING" || to == "TEXT" || to == "COMPOUND_TEXT" )
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
	std::vector<uint8_t> d;
	std::string mtype;

	return std::make_pair( std::move( d ), std::move( mtype ) );
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

uint8_t
system::modifier_state( void )
{
	char keys[32];
	memset( keys, 0, sizeof(keys) );

	XQueryKeymap( _display.get(), keys );

	uint8_t r = 0;
	static_assert( modifier::LEFT_CTRL == 0x01 &&
				   modifier::LEFT_SHIFT == 0x02 &&
				   modifier::LEFT_ALT == 0x04 &&
				   modifier::LEFT_META == 0x08 &&
				   modifier::RIGHT_CTRL == 0x10 &&
				   modifier::RIGHT_SHIFT == 0x20 &&
				   modifier::RIGHT_ALT == 0x40 &&
				   modifier::RIGHT_META == 0x80, "Modifiers incorrectly mapped" );

	static const KeySym modkeys[8] = {
		XK_Control_L, XK_Shift_L, XK_Alt_L, XK_Super_L,
		XK_Control_R, XK_Shift_R, XK_Alt_R, XK_Super_R 
	};
	for ( uint8_t i = 0; i < 8; ++i )
	{
		KeyCode kc = XKeysymToKeycode( _display.get(), modkeys[i] );
		if ( ( ( keys[kc >> 3] >> (kc & 7) ) & 1 ) != 0 )
			r |= (1 << i);
	}
	return r;
}

////////////////////////////////////////

bool
system::query_mouse( uint8_t &buttonMask, uint8_t &modifiers, coord_type &x, coord_type &y,int &screen )
{
	Window rt, child;
	int rx, ry, wx, wy;
	unsigned int mask;

	for ( int s = 0, nS = ScreenCount( _display.get() ); s < nS; ++s )
	{
		if ( XQueryPointer( _display.get(), RootWindow( _display.get(), s ), &rt, &child,
							&rx, &ry, &wx, &wy, &mask ) == True )
		{
			buttonMask = ((mask & Button1Mask) != 0) ? 0x01 : 0;
			buttonMask |= ((mask & Button2Mask) != 0) ? 0x02 : 0;
			buttonMask |= ((mask & Button3Mask) != 0) ? 0x04 : 0;
			buttonMask |= ((mask & Button4Mask) != 0) ? 0x08 : 0;
			buttonMask |= ((mask & Button5Mask) != 0) ? 0x10 : 0;

			modifiers = ( (((mask & ControlMask) != 0) ? 0x01 : 0) |
						  (((mask & ShiftMask) != 0) ? 0x02 : 0) );
			// the rest are Mod[1-5]Mask....

			x = rx;
			y = ry;
			screen = s;
			return true;
		}
	}
	buttonMask = 0;
	modifiers = 0;
	x = 0;
	y = 0;
	screen = -1;
	return false;
}

////////////////////////////////////////

} }
