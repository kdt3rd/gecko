//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <iostream>
#include <cmath>
#include <type_traits>

namespace base
{

////////////////////////////////////////

/// @brief Point.
//template <typename T>
class point
{
public:
//	typedef T coord_type;
	typedef double coord_type;

	/// @brief Default constructor.
	/// Create the point (0,0) (the origin).
	constexpr point( void ) = default;

	/// @brief Point constructor.
	constexpr point( coord_type xx, coord_type yy )
		: _x( xx ), _y( yy )
	{
	}

	/// @brief Copy constructor.
	constexpr point( const point &p ) = default;

	/// @brief Move constructor.
	constexpr point( point &&p ) noexcept = default;

	/// @brief Destructor.
	~point( void ) = default;

	/// @brief Copy assignment.
	point &operator=( const point &p ) = default;

	/// @brief Move assignment.
	point &operator=( point &&p ) noexcept = default;

	/// @brief X coordinate of the point.
	constexpr coord_type x( void ) const { return _x; }

	/// @brief Y coordinate of the point.
	constexpr coord_type y( void ) const { return _y; }

	/// @brief Set the coordinates of the point.
	void set( coord_type xx, coord_type yy )
	{
		_x = xx;
		_y = yy;
	}

	/// @brief Move the point by (dx,dy).
	void move_by( coord_type dx, coord_type dy )
	{
		_x += dx;
		_y += dy;
	}

	/// @brief Set the X coordinate of the point.
	void set_x( coord_type xx ) { _x = xx; }

	/// @brief Set the Y coordinate of the point.
	void set_y( coord_type yy ) { _y = yy; }

	/// @brief Move the point by (d.x(),d.y()).
	constexpr point delta( const point &d ) const
	{
		return { _x - d._x, _y - d._y };
	}

	/// @brief Add the coordinates of this and p.
	constexpr point operator+( const point &p ) const
	{
		return { _x + p._x, _y + p._y };
	}

	/// @brief Subtract the coordinates of p from this.
	constexpr point operator-( const point &p ) const
	{
		return { _x - p._x, _y - p._y };
	}

	/// @brief Scale point by v.
	constexpr point operator*( coord_type v ) const
	{
		return { _x * v, _y * v };
	}

	/// @brief Dot product.
	constexpr coord_type operator*( const point &p ) const
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
	static inline coord_type distance_squared( const point &p1, const point &p2 )
	{
		coord_type dx = p1.x() - p2.x();
		coord_type dy = p1.y() - p2.y();
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
		return { static_cast<coord_type>( r * std::cos( a ) ), static_cast<coord_type>( r * std::sin( a ) ) };
	}

	/// @brief Signed area of triangle (p1, p2, p3)
	static coord_type signed_area( const point &p1, const point &p2, const point &p3 )
	{
		coord_type a = p1.x() * ( p2.y() - p3.y() );
		coord_type b = p2.x() * ( p3.y() - p1.y() );
		coord_type c = p3.x() * ( p1.y() - p2.y() );
		return a + b + c;
	}

	/// @brief Area of triangle (p1, p2, p3)
	static coord_type area( const point &p1, const point &p2, const point &p3 )
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
		coord_type a = signed_area( p1, p2, p3 );
		return a > 0 ? orientation::COUNTERCLOCKWISE : ( a < 0 ? orientation::CLOCKWISE : orientation::COLLINEAR );
	}

private:
	coord_type _x = coord_type(0), _y = coord_type(0);
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
template <typename F>
inline point operator*( F v, const point &p )
{
	return p * static_cast<point::coord_type>( v );
}

////////////////////////////////////////

} // namespace base

