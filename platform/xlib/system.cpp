
#include "system.h"
#include "screen.h"
#include "window.h"
#include "timer.h"
#include "font_manager.h"
#include "dispatcher.h"

#include <platform/platform.h>
#include <core/contract.h>
#include <stdexcept>

////////////////////////////////////////

namespace {

int
xErrorCB( Display *d, XErrorEvent *e )
{
	char errorBuf[4096];

	XGetErrorText( d, e->error_code, errorBuf, 4096 );
	std::cerr << "ERROR: Xlib Error"
			  << "\n  Major/Minor: " << e->request_code << " / " << e->minor_code
			  << "\n   Error code: " << e->error_code
			  << "\n      Message: " << errorBuf << std::endl;

	return 0;
}

int
xIOErrorCB( Display *d )
{
	std::cerr << "ERROR: I/O error w/ X server (connection lost?)" << std::endl;
	exit( -1 );
}

}


namespace xlib
{

////////////////////////////////////////

system::system( void )
		: platform::system( "x11", "X11/XLib" )
{
	XSetErrorHandler( &xErrorCB );
	XSetIOErrorHandler( &xIOErrorCB );

	_display.reset( XOpenDisplay( nullptr ), &XCloseDisplay );
	if ( ! _display )
		throw std::runtime_error( "no X display" );

	_screens.resize( ScreenCount( _display.get() ) );
	for ( int i = 0; i < ScreenCount( _display.get() ); ++i )
		_screens[0] = std::make_shared<screen>( _display, i );

	_keyboard = std::make_shared<keyboard>();
	_mouse = std::make_shared<mouse>();
	_font_manager = std::make_shared<font_manager>();
	_dispatcher = std::make_shared<dispatcher>( _display, _keyboard, _mouse );
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>( _display );
	_dispatcher->add_window( ret );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<platform::timer> system::new_timer( void )
{
	return std::make_shared<timer>();
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

std::shared_ptr<platform::font_manager> system::get_font_manager( void )
{
	return _font_manager;
}

////////////////////////////////////////

}
