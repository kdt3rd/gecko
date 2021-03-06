// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "system.h"

#include "dispatcher.h"
#include "font_manager.h"
#include "screen.h"
#include "timer.h"
#include "window.h"

#include <core/contract.h>
#include <platform/platform.h>
#include <stdexcept>

namespace platform
{
namespace xcb
{
////////////////////////////////////////

system::system( void ) : platform::system( "xcb", "XCB" )
{
    int prefScreen = 0;
    _connection    = xcb_connect( nullptr, &prefScreen );
    if ( xcb_connection_has_error( _connection ) )
        throw std::runtime_error( "no xbc connection" );

    const xcb_setup_t *setup = xcb_get_setup( _connection );
    precondition( setup, "not xcb setup" );

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator( setup );
    _screen                    = iter.data;

    while ( iter.rem )
    {
        _screens.push_back( std::make_shared<screen>( iter.data ) );
        xcb_screen_next( &iter );
    }

    _keyboard     = std::make_shared<keyboard>( _connection );
    _mouse        = std::make_shared<mouse>();
    _font_manager = std::make_shared<font_manager>();
    _dispatcher =
        std::make_shared<dispatcher>( _connection, _keyboard, _mouse );
}

////////////////////////////////////////

system::~system( void ) { xcb_disconnect( _connection ); }

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
    auto ret = std::make_shared<window>( _connection, _screen );
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

std::shared_ptr<platform::mouse> system::get_mouse( void ) { return _mouse; }

////////////////////////////////////////

std::shared_ptr<platform::font_manager> system::get_font_manager( void )
{
    return _font_manager;
}

////////////////////////////////////////

} // namespace xcb
} // namespace platform
