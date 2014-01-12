
#pragma once

#include <iostream>
#include <cmath>

namespace core
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

	void move_by( double xx, double yy )
	{
		_x += xx;
		_y += yy;
	}

	void set_x( double xx ) { _x = xx; }
	void set_y( double yy ) { _y = yy; }

	point delta( const point &p ) const
	{
		return { _x - p._x, _y - p._y };
	}
	
	point operator+( const point &p ) const
	{
		return { _x + p._x, _y + p._y };
	}

	point operator-( const point &p ) const
	{
		return { _x - p._x, _y - p._y };
	}

	point operator*( double v ) const
	{
		return { _x * v, _y * v };
	}

	static inline double distance_squared( const point &p1, const point &p2 )
	{
		double dx = p1.x() - p2.x();
		double dy = p1.y() - p2.y();
		return dx * dx + dy * dy;
	}

	static inline double distance( const point &p1, const point &p2 )
	{
		return std::sqrt( distance_squared( p1, p2 ) );
	}

	static point polar( double r, double a )
	{
		return { r * std::cos( a ), r * std::sin( a ) };
	}

private:
	double _x = 0.0, _y = 0.0;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const point &p )
{
	out << p.x() << ',' << p.y();
	return out;
}

////////////////////////////////////////

}

