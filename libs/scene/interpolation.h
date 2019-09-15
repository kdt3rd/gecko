// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

#include "time.h"

////////////////////////////////////////

namespace scene
{

enum class interp_style : uint8_t
{
    constant = 0,
    linear,
    gamma,
    ease,
    ease_in,
    ease_out,
    cubic_hermite,
    uniform_catmullrom,
    chordal_catmullrom,
    centripetal_catmullrom,
    cubic,
    bezier,
    quintic,
    expression,
    custom
};

///
/// @brief Class interpolation provides...
///
class interpolation
{
public:
    using calc_type = double;

    interpolation( void );
    virtual ~interpolation( void );

    virtual calc_type
    offset( const time &start, const time &end, const time &t ) const = 0;

    virtual size_t storage_size( void ) const   = 0;
    virtual size_t control_points( void ) const = 0;

    // TBD: add support for custom
    static std::unique_ptr<interpolation> create( interp_style i );

private:
};

} // namespace scene
