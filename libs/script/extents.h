//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/units.h>

namespace script
{

using extent_type = float;
using points = base::units::points<extent_type>;

//using extent_type = double;

////////////////////////////////////////

/// @brief Extents of a glyph or string.
struct text_extents
{
	points x_bearing = points(0);
	points y_bearing = points(0);
	points width = points(0);
	points height = points(0);
	points x_advance = points(0);
	points y_advance = points(0);
};

////////////////////////////////////////

/// @brief Extents of a font.
struct font_extents
{
	points ascent = points(0);
	points descent = points(0);
	points width = points(0);
	points height = points(0);
	points max_x_advance = points(0);
	points max_y_advance = points(0);
};

////////////////////////////////////////

} // namespace script
