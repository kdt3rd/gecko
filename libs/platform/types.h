// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstddef>
#include <cstdint>

// need to include this prior to the others below such that the
// template routines are properly defined since we're using this as a
// type for them...
// clang-format off
#include <base/units.h>

#include <base/point.h>
#include <base/rect.h>
#include <base/size.h>
// clang-format on

////////////////////////////////////////

namespace platform
{
using dots_per_unit = base::fsize;

/// physical measurement types (i.e. actual length)
using phys_unit  = base::units::millimeters<float>;
using phys_point = base::point<phys_unit, 2>;
using phys_size  = base::size<phys_unit>;
using phys_rect  = base::rect<phys_unit>;

// Lowest common denominator in real life so far is int16 in the X
// protocol for drawing rectangles, etc
using coord_type = int16_t;
//using size_type = uint16_t;

using point = base::point<coord_type, 2>;
using size  = base::size<coord_type>;
using rect  = base::rect<coord_type>;

} // namespace platform
