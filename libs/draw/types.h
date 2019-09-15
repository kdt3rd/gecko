// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

// need to include this prior to the others below such that the
// template routines are properly defined since we're using this as a
// type for them...
// clang-format off
#include <base/units.h>
#include <base/rect.h>

#include <gl/color.h>
#include <gl/vector.h>
// clang-format on

////////////////////////////////////////

namespace draw
{
using color = gl::color;

//using dim = float;
using dim    = base::units::millimeters<float>;
using rect   = base::rect<dim>;
using point  = rect::point_type;
using size   = rect::size_type;
using point3 = base::point<dim, 3>;

constexpr inline float to_api( const dim &d ) { return d.count(); }
inline gl::vec2        to_api( const point &p )
{
    return gl::vec2( p[0].count(), p[1].count() );
}
inline gl::vec2 to_api( const size &s )
{
    return gl::vec2( s.w().count(), s.h().count() );
}

} // namespace draw
