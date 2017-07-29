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

#include <platform/platform.h>
#include <base/contract.h>
#include <base/env.h>
#include <stdexcept>

////////////////////////////////////////

namespace {

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
		throw std::runtime_error( "Current locale not supported by X" );

	if ( XSetLocaleModifiers( "@im=none" ) == nullptr )
		throw std::runtime_error( "Unable to set locale modifiers for Xlib" );

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
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

std::shared_ptr<::platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>( _display );
	_dispatcher->add_window( ret );
	return ret;
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
