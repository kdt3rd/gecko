// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/cursor.h>

////////////////////////////////////////

namespace platform
{
namespace cocoa
{
class cursor : public ::platform::cursor
{
public:
    explicit cursor( id c );

    cursor( void )            = delete;
    cursor( const cursor &c ) = delete;
    cursor( cursor &&c )      = delete;
    cursor &operator=( const cursor &c ) = delete;
    cursor &operator=( cursor &&c ) = delete;

    ~cursor( void ) override;

    bool supports_color( void ) const override;
    bool supports_animation( void ) const override;

    bool is_animated( void ) const override;

    id handle( void ) const { return _handle; }

private:
    id _handle;
};

} // namespace cocoa
} // namespace platform
