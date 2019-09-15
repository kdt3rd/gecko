// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/units.h>

namespace script
{
using extent_type = float;
using points      = base::units::points<extent_type>;

//using extent_type = double;

////////////////////////////////////////

/// @brief Extents of a glyph or string.
struct text_extents
{
    points x_bearing = points( 0 );
    points y_bearing = points( 0 );
    points width     = points( 0 );
    points height    = points( 0 );
    points x_advance = points( 0 );
    points y_advance = points( 0 );
};

////////////////////////////////////////

/// @brief Extents of a font.
struct font_extents
{
    points ascent        = points( 0 ); // distance above the baseline
    points descent       = points( 0 ); // distance below the baseline
    points width         = points( 0 ); // maximum width of all glyphs
    points height        = points( 0 );
    points max_x_advance = points( 0 );
    points max_y_advance = points( 0 );
};

////////////////////////////////////////

} // namespace script
