// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "system.h"

#include "dispatcher.h"
#include "font_manager.h"
#include "screen.h"
#include "timer.h"
#include "window.h"

#include <base/contract.h>
#include <platform/platform.h>
#include <stdexcept>

namespace platform
{
namespace dummy
{
////////////////////////////////////////

system::system( void ) : platform::system( "dummy", "Dummy" )
{
    _keyboard     = std::make_shared<keyboard>();
    _mouse        = std::make_shared<mouse>();
    _font_manager = std::make_shared<font_manager>();
    _dispatcher   = std::make_shared<dispatcher>( _keyboard, _mouse );
}

////////////////////////////////////////

system::~system( void ) {}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
    auto ret = std::make_shared<window>();
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

} // namespace dummy
} // namespace platform
