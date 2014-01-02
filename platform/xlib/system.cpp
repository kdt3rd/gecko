
#include "system.h"
#include "screen.h"
#include "window.h"
#include "timer.h"
#include "font_manager.h"
#include "dispatcher.h"

#include <platform/platform.h>
#include <core/contract.h>
#include <stdexcept>


namespace xlib
{

////////////////////////////////////////

system::system( void )
	: platform::system( "x11", "X11/XLib" )
{
	_display = XOpenDisplay( nullptr );
	if ( !_display )
		throw std::runtime_error( "no X display" );

	_keyboard = std::make_shared<keyboard>( _display );
	_mouse = std::make_shared<mouse>();
	_font_manager = std::make_shared<font_manager>();
	_dispatcher = std::make_shared<dispatcher>( _display, _keyboard, _mouse );
}

////////////////////////////////////////

system::~system( void )
{
	XCloseDisplay( _display );
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
