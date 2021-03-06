// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/mouse.h>

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

/// @brief Cocoa implementation of mouse.
class mouse : public ::platform::mouse
{
public:
    mouse( ::platform::system *s );
    ~mouse( void ) override;

    void start( void ) override;
    void cancel( void ) override;
    void shutdown( void ) override;
    wait poll_object( void ) override;
    bool poll_timeout( duration &when, const time_point &curtime ) override;
    void emit( const time_point &curtime ) override;
};

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
