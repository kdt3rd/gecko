// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/screen.h>
#include <xcb/xcb.h>

namespace platform
{
namespace xcb
{
////////////////////////////////////////

/// @brief XCB implementation of platform::screen
class screen : public platform::screen
{
public:
    /// @brief Constructor.
    screen( xcb_screen_t *scr );
    ~screen( void );

    draw::size bounds( void ) override;

    /// @brief XCB identifier for the screen.
    xcb_screen_t *id( void ) { return _screen; }

private:
    xcb_screen_t *_screen;
};

////////////////////////////////////////

} // namespace xcb
} // namespace platform
