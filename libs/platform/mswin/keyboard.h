// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/keyboard.h>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

class keyboard : public ::platform::keyboard
{
public:
    keyboard( ::platform::system *s );
    virtual ~keyboard( void );

    void start( void ) override;
    void cancel( void ) override;
    void shutdown( void ) override;
    wait poll_object( void ) override;
    bool poll_timeout( duration &when, const time_point &curtime ) override;
    void emit( const time_point &curtime ) override;
};

////////////////////////////////////////

} // namespace mswin
} // namespace platform
