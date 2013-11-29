
#pragma once

#include <cmath>

namespace draw
{

////////////////////////////////////////

/// @brief Point.
class point
{
public:
	constexpr point( void )
	{
	}

	constexpr point( double xx, double yy )
		: _x( xx ), _y( yy )
	{
	}

	constexpr double x( void ) const { return _x; }
	constexpr double y( void ) const { return _y; }

	void set( double xx, double yy )
	{
		_x = xx;
		_y = yy;
	}

	void set_x( double xx ) { _x = xx; }
	void set_y( double yy ) { _y = yy; }

	point delta( const point &p ) const
	{
		return { _x - p._x, _y - p._y };
	}

	static inline double distance( const point &p1, const point &p2 )
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

