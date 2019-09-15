// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/keyboard.h>

namespace platform
{
namespace wayland
{
////////////////////////////////////////

/// @brief Wayland implementation of platform::keyboard.
class keyboard : public ::platform::keyboard
{
public:
    /// @brief Constructor.
    keyboard( ::platform::system *s );
    ~keyboard( void );

    void start( void ) override;
    void cancel( void ) override;
    void shutdown( void ) override;
    wait poll_object( void ) override;
    bool poll_timeout( duration &when, const time_point &curtime ) override;
    void emit( const time_point &curtime ) override;
};

////////////////////////////////////////

} // namespace wayland
} // namespace platform
