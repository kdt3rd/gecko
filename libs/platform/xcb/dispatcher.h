// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "window.h"

#include <map>
#include <memory>
#include <platform/dispatcher.h>

namespace platform
{
namespace xcb
{
////////////////////////////////////////

/// @brief XCB implementation of platform::dispatcher.
///
/// Dispatcher implemented using XCB.
class dispatcher : public platform::dispatcher
{
public:
    /// @brief Constructor.
    dispatcher(
        xcb_connection_t *               c,
        const std::shared_ptr<keyboard> &k,
        const std::shared_ptr<mouse> &   m );
    ~dispatcher( void );

    int  execute( void ) override;
    void exit( int code ) override;

    /// @brief Add a window.
    ///
    /// Add a window for the dispatcher to handle events.
    void add_window( const std::shared_ptr<window> &w );

private:
    int                                             _exit_code  = 0;
    xcb_connection_t *                              _connection = nullptr;
    xcb_atom_t                                      _atom_wm_protocols;
    xcb_atom_t                                      _atom_delete_window;
    std::shared_ptr<keyboard>                       _keyboard;
    std::shared_ptr<mouse>                          _mouse;
    std::map<xcb_window_t, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

} // namespace xcb
} // namespace platform
