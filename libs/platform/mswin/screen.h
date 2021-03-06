// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/size.h>
#include <color/color.h>
#include <platform/screen.h>
#include <string>
#include <windows.h>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

/// @brief Microsoft Windows screen.
class screen : public platform::screen
{
public:
    screen( HMONITOR mon );
    virtual ~screen( void );

    bool is_default( void ) const override;

    bool is_managed( void ) const override;

    bool is_remote( void ) const override;

    rect bounds( bool avail ) const override;

    dots_per_unit dpi( void ) const override;
    dots_per_unit dpmm( void ) const override;

    double refresh_rate( void ) const override;

    const color::standard_definition &display_standard( void ) const override;

    void
    override_display_standard( const color::standard_definition & ) override;

private:
    HMONITOR    _monitor;
    std::string _disp_devname;
    bool        _is_primary;

    color::standard_definition _standard;
};

////////////////////////////////////////

} // namespace mswin
} // namespace platform
