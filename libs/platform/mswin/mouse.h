// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <platform/mouse.h>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

class mouse : public ::platform::mouse
{
public:
    mouse( ::platform::system *s );
    virtual ~mouse( void );

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
