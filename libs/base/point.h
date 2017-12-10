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
template <typename T = double>
class point
{
public:
	static_assert( std::is_arithmetic<T>::value, "point should be a signed value" );
	static_assert( std::is_signed<T>::value, "point should be a signed value" );
	typedef T coord_type;

	/// @brief Default constructor.
	/// Create the point (0,0) (the origin).
	constexpr point( void ) = default;
	constexpr point( const point & ) = default;
	constexpr point( point && ) noexcept = default;
	point &operator=( const point & ) = default;
	point &operator=( point && ) noexcept = default;
	~point( void ) = default;

	/// @brief Point constructor.
	constexpr point( coord_type xx, coord_type yy )
		: _x( xx ), _y( yy )
	{
	}

	/// @brief construct a point with a differently typed point.
	///
	/// Requires explicit construction to avoid blind conversion
	template <typename U>
	explicit constexpr point( const point<U> &o )
		: _x( static_cast<coord_type>( o.x() ) ),
		  _y( static_cast<coord_type>( o.y() ) )
	{}

	/// @brief explicit cast operator
	///
	/// This enables conversion to a different point type, but
	/// requires explicit programmer specification of such.
	template <typename U>
	explicit inline operator point<U>() const { return point<U>( *this ); }

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
	static inline
	typename std::conditional<sizeof(coord_type) <= sizeof(double), double, long double>::type
	distance( const point &p1, const point &p2 )
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
template <typename T>
inline std::ostream &operator<<( std::ostream &out, const point<T> &p )
{
	out << p.x() << ',' << p.y();
	return out;
}

////////////////////////////////////////

/// @brief Scale point p by v.
template <typename F, typename T>
inline point<T> operator*( F v, const point<T> &p )
{
	return p * static_cast<typename point<T>::coord_type>( v );
}

using fpoint = point<float>;
using dpoint = point<double>;
using ipoint = point<int32_t>;
using lpoint = point<int64_t>;

////////////////////////////////////////

} // namespace base

