// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "dispatcher.h"

#include <X11/Xlib.h>
#include <platform/system.h>

namespace platform
{
class cursor;

namespace xlib
{
////////////////////////////////////////

/// @brief Xlib implementation of platform::system.
class system : public ::platform::system
{
public:
    system( const std::string &d );
    ~system( void );

    bool is_working( void ) const override;

    std::vector<std::shared_ptr<::platform::screen>> screens( void ) override;
    const std::shared_ptr<::platform::screen> &default_screen( void ) override;

    std::shared_ptr<::platform::cursor> new_cursor( void ) override;
    std::shared_ptr<::platform::cursor>
    builtin_cursor( standard_cursor sc ) override;

    ////////////////////////////////////////

    void set_selection( selection sel ) override;

    std::pair<std::vector<uint8_t>, std::string> query_selection(
        selection_type                  sel,
        const std::vector<std::string> &allowedMimeTypes =
            std::vector<std::string>(),
        const std::string &clipboardName = std::string() ) override;

    std::pair<std::vector<uint8_t>, std::string> query_selection(
        selection_type                 sel,
        const selection_type_function &chooseMimeType,
        const std::string &            clipboardName = std::string() ) override;

    const std::vector<std::string> &default_string_types( void ) override;
    selection_type_function         default_string_selector( void ) override;
    mime_converter                  default_string_converter( void ) override;

    ////////////////////////////////////////

    void begin_drag(
        selection                                  sel,
        const std::shared_ptr<::platform::cursor> &cursor =
            std::shared_ptr<::platform::cursor>() ) override;

    std::pair<std::vector<uint8_t>, std::string> query_drop(
        const selection_type_function &chooseMimeType =
            selection_type_function() ) override;

    std::shared_ptr<::platform::menu> new_system_menu( void ) override;
    std::shared_ptr<::platform::tray> new_system_tray_item( void ) override;

    std::shared_ptr<::platform::window> new_window(
        window_type wintype = window_type::normal,
        const std::shared_ptr<::platform::screen> &s =
            std::shared_ptr<::platform::screen>() ) override;

    void
    destroy_window( const std::shared_ptr<::platform::window> &w ) override;

    std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;

    //uint8_t modifier_state( void ) override;
    //bool query_mouse( uint8_t &buttonMask, uint8_t &modifiers, coord_type &x, coord_type &y, int &screen ) override;

private:
    std::shared_ptr<Display>    _display;
    std::shared_ptr<dispatcher> _dispatcher;

    std::vector<std::shared_ptr<::platform::screen>> _screens;

    std::map<::platform::standard_cursor, std::shared_ptr<::platform::cursor>>
        _cursors;
};

////////////////////////////////////////

} // namespace xlib
} // namespace platform
