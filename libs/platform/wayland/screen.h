// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <color/color.h>
#include <memory>
#include <platform/screen.h>

namespace platform
{
namespace wayland
{
////////////////////////////////////////

/// @brief wayland implementation of platform::screen
class screen : public ::platform::screen
{
public:
    /// @brief Constructor.
    screen( void );
    ~screen( void );

    bool is_default( void ) const override;

    bool is_managed( void ) const override;

    bool is_remote( void ) const override;

    rect bounds( bool active ) const override;

    dots_per_unit dpi( void ) const override;
    dots_per_unit dpmm( void ) const override;

    double refresh_rate( void ) const override;

    const color::standard_definition &display_standard( void ) const override;
    void
    override_display_standard( const color::standard_definition & ) override;

private:
    color::standard_definition _standard;
};

////////////////////////////////////////

} // namespace wayland
} // namespace platform
