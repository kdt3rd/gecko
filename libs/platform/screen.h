// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "types.h"

#include <memory>

namespace color
{
class standard_definition;
}

namespace platform
{
////////////////////////////////////////

/// @brief Screen device.
///
/// A screen device
class screen
{
public:
    /// @brief Constructor.
    ///
    /// TODO: add methods to query color management (i.e. ICM under
    /// windows), change the resolution / display rate on the fly,
    /// etc.
    screen( void );

    /// @brief Destructor.
    virtual ~screen( void );

    phys_unit  to_physical_horiz( coord_type x );
    phys_unit  to_physical_vert( coord_type y );
    phys_point to_physical( const point &p );
    phys_size  to_physical( const size &p );
    phys_rect  to_physical( const rect &p );

    coord_type to_native_horiz( const phys_unit &u );
    coord_type to_native_vert( const phys_unit &u );
    point      to_native( const phys_point &u );
    size       to_native( const phys_size &u );
    rect       to_native( const phys_rect &u );

    /// @brief default state
    ///
    /// @return Bool indicating whether this screen should be
    /// considered the 'default' screen
    virtual bool is_default( void ) const = 0;

    /// @brief determine if screen has a window manager running
    ///
    /// if this returns false, the screen should be used as a full
    /// screen-only screen.
    virtual bool is_managed( void ) const = 0;

    /// @brief determine if the screen is a remote screen.
    ///
    /// The best example of this is if the process is running on a
    /// remote machine (or a VM), and connecting to the X server via
    /// the network.
    virtual bool is_remote( void ) const = 0;

    /// @brief Screen bounds.
    ///
    /// The bounds (size) of the screen, either the full (avail ==
    /// false) or the available user space (avail == true)
    ///
    /// @return The size of the screen
    virtual rect bounds( bool avail ) const = 0;
    phys_rect    physical_bounds( bool avail );

    /// @brief Screen DPI
    ///
    /// Returns the horizontal and vertical DPI for the screen
    ///
    /// This is a floating point number in case the screen is actually
    /// that precise in reporting.
    virtual dots_per_unit dpi( void ) const = 0;

    /// @brief Screen dots per millimeter
    ///
    /// SI units instead of imperial
    virtual dots_per_unit dpmm( void ) const = 0;

    /// @brief returns the current refresh rate of the screen
    ///
    /// This should be measured in cycles / second
    virtual double refresh_rate( void ) const = 0;

    /// @brief Screen display standard
    ///
    /// The output color space. This should be used to transform all
    /// UI colors from their specified space to the output space
    /// appropriate for this screen.
    virtual const color::standard_definition &
    display_standard( void ) const = 0;

    /// @brief Force display standard to be used
    ///
    /// This can be used to force a particular display standard be
    /// used for the screen
    virtual void
    override_display_standard( const color::standard_definition & ) = 0;
};

////////////////////////////////////////

} // namespace platform
