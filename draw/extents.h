
#pragma once

namespace draw
{

////////////////////////////////////////

struct text_extents
{
	double x_bearing;
	double y_bearing;
	double width;
	double height;
	double x_advance;
	double y_advance;
};

////////////////////////////////////////

struct font_extents
{
	double ascent;
	double descent;
	double height;
	double max_x_advance;
	double max_y_advance;
};

////////////////////////////////////////

}

