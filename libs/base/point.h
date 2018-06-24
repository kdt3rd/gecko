//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <iostream>
#include <cmath>
#include "type_util.h"

namespace base
{

////////////////////////////////////////

/// @brief Point.
///
/// There are extrinsic functions provided for some useful operations
/// on points, however are not required for a simple point. As such,
/// the point itself has looser type requirements than those
/// operations.
template <typename T = double>
class point
{
public:
	static_assert( has_arithmetic_ops<T>::value, "point should be composed of an arithmetic value type" );
	using coord_type = T;

	/// @brief Default constructor.
	/// Create the point (0,0) (the origin).
	constexpr point( void ) = default;
	constexpr point( const point & ) = default;
	constexpr point( point && ) noexcept = default;
	point &operator=( const point & ) = default;
	point &operator=( point && ) noexcept = default;
	~point( void ) = default;

	/// @brief Point constructor.
	constexpr inline point( coord_type xx, coord_type yy )
		: _x( xx ), _y( yy )
	{
	}

	/// @brief construct a point with a differently typed point.
	///
	/// Requires explicit construction to avoid blind conversion
	template <typename U>
	explicit constexpr inline point( const point<U> &o )
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

	constexpr bool operator<( const point &o ) const
	{
		return x() < o.x() || ( std::equal_to<coord_type>( x(), o.x() ) && y() < o.y() );
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

////////////////////////////////////////

template <typename T>
inline std::enable_if_t<std::is_floating_point<T>::value, point<T>>
round( const point<T> &p )
{
	return point<T>( std::round( p.x() ), std::round( p.y() ) );
}

template <typename T>
inline std::enable_if_t<std::is_integral<T>::value, point<T>>
round( const point<T> &p )
{
	return p;
}

template <typename T>
inline std::enable_if_t<! std::is_floating_point<T>::value &&
						! std::is_integral<T>::value, point<T>>
round( const point<T> &p )
{
	return point<T>( static_cast<T>( std::round( p.x() ) ),
					 static_cast<T>( std::round( p.y() ) ) );
}

template <typename T>
inline point<long> lround( const point<T> &p )
{
	return point<long>( std::lround( p.x() ), std::lround( p.y() ) );
}

template <typename T>
inline point<long long> llround( const point<T> &p )
{
	return point<long long>( std::llround( p.x() ), std::llround( p.y() ) );
}

template <typename T>
inline point<T> abs( const point<T> &a )
{
	return point<T>( std::abs( a.x() ), std::abs( a.y() ) );
}

template <typename T>
inline point<T> ceil( const point<T> &a )
{
	return point<T>( std::ceil( a.x() ), std::ceil( a.y() ) );
}

template <typename T>
inline point<T> floor( const point<T> &a )
{
	return point<T>( std::floor( a.x() ), std::floor( a.y() ) );
}

template <typename T>
inline point<T> max( const point<T> &a, const point<T> &b )
{
	return point<T>( std::max( a.x(), b.x() ), std::max( a.y(), b.y() ) );
}

template <typename T>
inline point<T> min( const point<T> &a, const point<T> &b )
{
	return point<T>( std::min( a.x(), b.x() ), std::min( a.y(), b.y() ) );
}

/// @brief Calculate the distance squared between points p1 and p2.
template <typename T>
inline T distance_squared( const point<T> &p1, const point<T> &p2 )
{
	static_assert( std::is_signed<T>::value, "point coordinate type should be a signed value" );
	T dx = p1.x() - p2.x();
	T dy = p1.y() - p2.y();
	return dx * dx + dy * dy;
}

/// @brief Calculate the distance between points p1 and p2.
template <typename T>
inline typename std::conditional<std::is_floating_point<T>::value, T, double>::type
distance( const point<T> &p1, const point<T> &p2 )
{
	using result = typename std::conditional<std::is_floating_point<T>::value, T, double>::type;
	return std::sqrt( static_cast<result>( distance_squared( p1, p2 ) ) );
}

/// @brief Signed area of triangle (p1, p2, p3)
template <typename T>
inline T signed_area( const point<T> &p1, const point<T> &p2, const point<T> &p3 )
{
	static_assert( std::is_signed<T>::value, "point coordinate type should be a signed value" );
	T a = p1.x() * ( p2.y() - p3.y() );
	T b = p2.x() * ( p3.y() - p1.y() );
	T c = p3.x() * ( p1.y() - p2.y() );
	return a + b + c;
}

/// @brief Area of triangle (p1, p2, p3)
template <typename T>
inline T area( const point<T> &p1, const point<T> &p2, const point<T> &p3 )
{
	return std::abs( signed_area( p1, p2, p3 ) );
}

/// @brief Orientation of points.
enum class point_orientation
{
	COUNTERCLOCKWISE,
	CLOCKWISE,
	COLLINEAR
};

/// @brief Which direction the points p1 -> p2 -> p3 are turning in.
template <typename T>
inline point_orientation turn( const point<T> &p1, const point<T> &p2, const point<T> &p3 )
{
	T a = signed_area( p1, p2, p3 );
	return ( ( a > T(0) ) ? point_orientation::COUNTERCLOCKWISE :
			 ( ( a < T(0) ) ? point_orientation::CLOCKWISE : point_orientation::COLLINEAR ) );
}

/// @brief Is point p inside the triangle (p1,p2,p3)?
template <typename T>
inline bool is_inside( const point<T> &p, const point<T> &p1, const point<T> &p2, const point<T> &p3 )
{
	point_orientation o1 = turn( p1, p2, p );
	point_orientation o2 = turn( p2, p3, p );
	point_orientation o3 = turn( p3, p1, p );
	return o1 == o2 && o2 == o3;
}

/// @brief Create a point using polar coordinates.
/// @param r distance of the point from the origin (radius).
/// @param a angle of the point, in radians.
/// \todo { if c++ ever adds a sincos or we move to our own math library, switch to sincos }
template<typename F1, typename F2, typename T = F1>
static point<T> polar( F1 r, F2 a )
{
	static_assert( std::is_floating_point<F1>::value, "polar requires floating point type" );
	static_assert( std::is_floating_point<F2>::value, "polar requires floating point type" );
	return point<T>( static_cast<T>( r * std::cos( a ) ),
					 static_cast<T>( r * std::sin( a ) ) );
}

////////////////////////////////////////

using fpoint = point<float>;
using dpoint = point<double>;
using ipoint = point<int32_t>;
using lpoint = point<int64_t>;

////////////////////////////////////////

} // namespace base


// provide std specialization of the functions above
namespace std
{

template <typename T>
inline base::point<T> round( const base::point<T> &a )
{
	return base::round( a );
}

template <typename T>
inline base::point<long> lround( const base::point<T> &a )
{
	return base::lround( a );
}

template <typename T>
inline base::point<long long> llround( const base::point<T> &a )
{
	return base::llround( a );
}

template <typename T>
inline base::point<T> abs( const base::point<T> &a )
{
	return base::abs( a );
}

template <typename T>
inline base::point<T> ceil( const base::point<T> &a )
{
	return base::ceil( a );
}

template <typename T>
inline base::point<T> floor( const base::point<T> &a )
{
	return base::floor( a );
}

} // namespace std
