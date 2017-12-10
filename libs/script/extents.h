//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

namespace script
{

using extent_type = float;
//using extent_type = double;

////////////////////////////////////////

/// @brief Extents of a glyph or string.
struct text_extents
{
	extent_type x_bearing = extent_type(0);
	extent_type y_bearing = extent_type(0);
	extent_type width = extent_type(0);
	extent_type height = extent_type(0);
	extent_type x_advance = extent_type(0);
	extent_type y_advance = extent_type(0);
};

////////////////////////////////////////

/// @brief Extents of a font.
struct font_extents
{
	extent_type ascent = extent_type(0);
	extent_type descent = extent_type(0);
	extent_type width = extent_type(0);
	extent_type height = extent_type(0);
	extent_type max_x_advance = extent_type(0);
	extent_type max_y_advance = extent_type(0);
};

////////////////////////////////////////

}

