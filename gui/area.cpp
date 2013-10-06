
#include "area.h"

////////////////////////////////////////

void area::set_xy( double x, double y )
{
	double w = width();
	double h = height();
	_x1 = x;
	_y1 = y;
	_x2 = _x1 + w;
	_y2 = _y1 + h;
}

////////////////////////////////////////

bool area::contains( double x, double y ) const
{
	return ( x >= _x1 && x <= _x2 ) && ( y >= _y1 && y <= _y2 );
}

////////////////////////////////////////

