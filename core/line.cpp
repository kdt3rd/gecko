
#include "line.h"

////////////////////////////////////////

void line::set_xy( double x, double y )
{
	double w = width();
	double h = height();
	_x1 = x;
	_y1 = y;
	_x2 = _x1 + w;
	_y2 = _y1 + h;
}

////////////////////////////////////////

