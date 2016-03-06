
#pragma once

#include <iostream>
#include <cmath>

namespace base
{

////////////////////////////////////////

/// @brief Point.
class point
{
public:
	/// @brief Default constructor.
	/// Create the point (0,0) (the origin).
	constexpr point( void )
	{
	}

	/// @brief Point constructor.
	constexpr point( double xx, double yy )
		: _x( xx ), _y( yy )
	{
	}

	/// @brief X coordinate of the point.
	constexpr double x( void ) const { return _x; }

	/// @brief Y coordinate of the point.
	constexpr double y( void ) const { return _y; }

	/// @brief Set the coordinates of the point.
	void set( double xx, double yy )
	{
		_x = xx;
		_y = yy;
	}

	/// @brief Move the point by (dx,dy).
	void move_by( double dx, double dy )
	{
		_x += dx;
		_y += dy;
	}

	/// @brief Set the X coordinate of the point.
	void set_x( double xx ) { _x = xx; }

	/// @brief Set the Y coordinate of the point.
	void set_y( double yy ) { _y = yy; }

	/// @brief Move the point by (d.x(),d.y()).
	point delta( const point &d ) const
	{
		return { _x - d._x, _y - d._y };
	}

	/// @brief Add the coordinates of this and p.
	point operator+( const point &p ) const
	{
		return { _x + p._x, _y + p._y };
	}

	/// @brief Subtract the coordinates of p from this.
	point operator-( const point &p ) const
	{
		return { _x - p._x, _y - p._y };
	}

	/// @brief Scale point by v.
	point operator*( double v ) const
	{
		return { _x * v, _y * v };
	}

	/// @brief Dot product.
	double operator*( const point &p ) const
	{
		return x() * p.x() + y() * p.y();
	}

	/// @brief Is point inside the triangle (p1,p2,p3)?
	bool is_inside( const point &p1, const point &p2, const point &p3 ) const
	{
		orientation o1 = turn( p1, p2, *this );
		orientation o2 = turn( p2, p3, *this );
		orientation o3 = turn( p3, p1, *this );
		return o1 == o2 && o2 == o3;
	}

	/// @brief Calculate the distance squared between points p1 and p2.
	static inline double distance_squared( const point &p1, const point &p2 )
	{
		double dx = p1.x() - p2.x();
		double dy = p1.y() - p2.y();
		return dx * dx + dy * dy;
	}

	/// @brief Calculate the distance between points p1 and p2.
	static inline double distance( const point &p1, const point &p2 )
	{
		return std::sqrt( distance_squared( p1, p2 ) );
	}

	/// @brief Create a point using polar coordinates.
	/// @param r distance of the point from the origin (radius).
	/// @param a angle of the point, in radians.
	template<typename F1, typename F2>
	static point polar( F1 r, F2 a )
	{
		static_assert( std::is_floating_point<F1>::value, "polar requires floating point type" );
		static_assert( std::is_floating_point<F2>::value, "polar requires floating point type" );
		return { static_cast<double>(r) * static_cast<double>( std::cos( a ) ), static_cast<double>(r) * static_cast<double>( std::sin( a ) ) };
	}

	/// @brief Signed area of triangle (p1, p2, p3)
	static double signed_area( const point &p1, const point &p2, const point &p3 )
	{
		double a = p1.x() * ( p2.y() - p3.y() );
		double b = p2.x() * ( p3.y() - p1.y() );
		double c = p3.x() * ( p1.y() - p2.y() );
		return a + b + c;
	}

	/// @brief Area of triangle (p1, p2, p3)
	static double area( const point &p1, const point &p2, const point &p3 )
	{
		return std::abs( signed_area( p1, p2, p3 ) );
	}

	/// @brief Orientation of points.
	enum class orientation
	{
		COUNTERCLOCKWISE,
		CLOCKWISE,
		COLLINEAR
	};

	/// @brief Which direction the points p1 -> p2 -> p3 are turning in.
	static orientation turn( const point &p1, const point &p2, const point &p3 )
	{
		double a = signed_area( p1, p2, p3 );
		return a > 0 ? orientation::COUNTERCLOCKWISE : ( a < 0 ? orientation::CLOCKWISE : orientation::COLLINEAR );
	}

private:
	double _x = 0.0, _y = 0.0;
};

////////////////////////////////////////

/// @brief Output operator for point.
inline std::ostream &operator<<( std::ostream &out, const point &p )
{
	out << p.x() << ',' << p.y();
	return out;
}

////////////////////////////////////////

/// @brief Scale point p by v.
inline point operator*( double v, const point &p )
{
	return p * v;
}

////////////////////////////////////////

}

