// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/keyboard.h>

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

/// @brief Cocoa implementation of keyboard.
class keyboard : public ::platform::keyboard
{
public:
    keyboard( ::platform::system *s );
    ~keyboard( void ) override;

    void update_mapping( void );

    void start( void ) override;
    void cancel( void ) override;
    void shutdown( void ) override;
    wait poll_object( void ) override;
    bool poll_timeout( duration &when, const time_point &curtime ) override;
    void emit( const time_point &curtime ) override;

    //	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );
    ::platform::scancode get_scancode( unsigned short code );

    const char *scancode_name( int scancode );

private:
    struct keylayout_wrapper;
    std::unique_ptr<keylayout_wrapper> _key_wrapper;
    char                               _uni_keys[64];
};

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
