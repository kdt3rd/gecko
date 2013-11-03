
#pragma once

#include <cmath>

namespace draw
{

////////////////////////////////////////

/// @brief Point.
class point
{
public:
	point( void )
	{
	}

	point( double xx, double yy )
		: _x( xx ), _y( yy )
	{
	}

	double x( void ) const { return _x; }
	double y( void ) const { return _y; }

	void set( double xx, double yy )
	{
		_x = xx;
		_y = yy;
	}

	void set_x( double xx ) { _x = xx; }
	void set_y( double yy ) { _y = yy; }

	static double distance( const point &p1, const point &p2 )
	{
		double dx = p1.x() - p2.x();
		double dy = p1.y() - p2.y();
		return std::sqrt( dx * dx + dy * dy );
	}

private:
	double _x = 0.0, _y = 0.0;
};

////////////////////////////////////////

}

