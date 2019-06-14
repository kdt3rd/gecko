// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "system.h"

#include "dispatcher.h"
#include "screen.h"
#include "window.h"

#include <base/contract.h>
#include <mutex>
#include <platform/menu.h>
#include <platform/tray.h>

namespace
{
BOOL CALLBACK
     monitorEnumCB( HMONITOR hMon, HDC hdcMon, LPRECT lprcMonitor, LPARAM userdata )
{
    std::vector<std::shared_ptr<::platform::screen>> *sl =
        reinterpret_cast<std::vector<std::shared_ptr<::platform::screen>> *>(
            userdata );
    sl->emplace_back( std::make_shared<platform::mswin::screen>( hMon ) );
    return true;
}

} // namespace

namespace platform
{
namespace mswin
{
////////////////////////////////////////

system::system( const std::string & )
    : platform::system( "mswin", "Microsoft Windows" )
{
    _dispatcher = std::make_shared<dispatcher>( this );

    EnumDisplayMonitors( NULL, NULL, monitorEnumCB, (LPARAM)&_screens );
}

////////////////////////////////////////

system::~system( void ) {}

////////////////////////////////////////

const std::shared_ptr<::platform::screen> &system::default_screen( void )
{
    return _screens.front();
}

////////////////////////////////////////

std::shared_ptr<cursor> system::new_cursor( void )
{
    std::cout << "implement new_cursor" << std::endl;
    return std::shared_ptr<cursor>();
}

////////////////////////////////////////

std::shared_ptr<cursor> system::builtin_cursor( standard_cursor sc )
{
    std::cout << "implement builtin_cursor" << std::endl;
    return std::shared_ptr<cursor>();
}

////////////////////////////////////////

void system::set_selection( selection sel )
{
    std::cout << "implement set_selection" << std::endl;
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string> system::query_selection(
    selection_type                  sel,
    const std::vector<std::string> &allowedMimeTypes,
    const std::string &             clipboardName )
{
    std::cout << "implement query_selection" << std::endl;
    return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string> system::query_selection(
    selection_type                 sel,
    const selection_type_function &mimeSelector,
    const std::string &            clipboardName )
{
    std::cout << "implement query_selection" << std::endl;
    return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

const std::vector<std::string> &system::default_string_types( void )
{
    static std::vector<std::string> mswinTypes{ "text/plain;charset=utf-8",
                                                "text/plain" };
    std::cout << "implement default_string_types" << std::endl;
    return mswinTypes;
}

////////////////////////////////////////

selection_type_function system::default_string_selector( void )
{
    auto selFun = []( const std::vector<std::string> &l ) -> std::string {
        std::string ret{ "text/plain;charset=utf-8" };
        if ( std::find( l.begin(), l.end(), ret ) != l.end() )
            return ret;
        ret = "text/plain";
        if ( std::find( l.begin(), l.end(), ret ) != l.end() )
            return ret;
        return std::string();
    };
    std::cout << "implement default_string_selector" << std::endl;
    return selection_type_function{ selFun };
}

////////////////////////////////////////

system::mime_converter system::default_string_converter( void )
{
    auto convFun = []( const std::vector<uint8_t> &d,
                       const std::string &         cur,
                       const std::string &to ) -> std::vector<uint8_t> {
        if ( base::begins_with( cur, "text/plain" ) )
        {
            if ( base::begins_with( to, "text/plain" ) )
                return d;
        }
        return std::vector<uint8_t>();
    };
    std::cout << "implement default_string_converter" << std::endl;
    return mime_converter{ convFun };
}

////////////////////////////////////////

void system::begin_drag( selection sel, const std::shared_ptr<cursor> &cursor )
{
    std::cout << "implement begin_drag" << std::endl;
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
system::query_drop( const selection_type_function &chooseMimeType )
{
    std::cout << "implement query_drop" << std::endl;
    return std::make_pair( std::vector<uint8_t>(), std::string() );
}

////////////////////////////////////////

std::shared_ptr<::platform::menu> system::new_system_menu( void )
{
    return std::make_shared<::platform::menu>();
}

////////////////////////////////////////

std::shared_ptr<::platform::tray> system::new_system_tray_item( void )
{
    return std::make_shared<::platform::tray>();
}

////////////////////////////////////////

std::shared_ptr<::platform::window> system::new_window(
    window_type wintype, const std::shared_ptr<::platform::screen> &s )
{
    auto ret = std::make_shared<window>( wintype, s ? s : default_screen() );
    _dispatcher->add_window( ret );
    return ret;
}

////////////////////////////////////////

void system::destroy_window( const std::shared_ptr<::platform::window> &w )
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

} // namespace mswin
} // namespace platform
