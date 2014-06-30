
#pragma once

namespace script
{

////////////////////////////////////////

/// @brief Extents of a glyph or string.
struct text_extents
{
	double x_bearing = 0.0;
	double y_bearing = 0.0;
	double width = 0.0;
	double height = 0.0;
	double x_advance = 0.0;
	double y_advance = 0.0;
};

////////////////////////////////////////

/// @brief Extents of a font.
struct font_extents
{
	double ascent = 0.0;
	double descent = 0.0;
	double width = 0.0;
	double height = 0.0;
	double max_x_advance = 0.0;
	double max_y_advance = 0.0;
};

////////////////////////////////////////

}

