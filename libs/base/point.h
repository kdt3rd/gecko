//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <iostream>
#include <cmath>
#include <iterator>
#include <array>
#include <initializer_list>
#include "contract.h"
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
template <typename T, size_t N>
class point
{
public:
	static_assert( has_arithmetic_ops<T>::value, "point should be composed of an arithmetic value type" );
	static_assert( N > 0, "expect non-zero array size for point");
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
	explicit inline point( coord_type x )
	{
		for (size_t i = 0; i != N; ++i )
			_v[i] = x;
	}

	/// @brief Point constructor.
	inline point( const std::array<coord_type, N> &a )
	{
		std::copy( std::begin( a ), std::end( a ), std::begin( _v ) );
	}

	/// @brief Point constructor.
	inline point( const std::initializer_list<coord_type> l )
	{
		auto i = l.begin();
		for ( size_t x = 0; x < std::min( l.size(), N ); ++x )
			_v[x] = *i++;
		for ( size_t x = l.size(); x < N; ++x )
			_v[x] = coord_type(0);
	}

	inline point( point &p )
	{
		std::copy( std::begin( p._v ), std::end( p._v ), std::begin( _v ) );
	}

	template <typename ... Args>
	explicit inline point( Args && ... p )
		: _v{ std::forward<Args>( p )... }
	{
		static_assert( sizeof...(Args) == N, "expect to initialize point with correct number of coords" );
	}

	/// @brief construct a point with a differently typed point.
	///
	/// Requires explicit construction to avoid blind conversion
	template <typename U>
	explicit inline point( const point<U, N> &o )
	{
		std::copy( o.begin(), o.end(), std::begin( _v ) );
	}

	template <typename U, size_t M>
	inline point( const point<U, M> &o )
	{
		static_assert( M <= N, "expect a compatible point that will result in no data loss" );
		std::copy( o.begin(), o.end(), std::begin( _v ) );
		for ( size_t i = M; i < N; ++i )
			_v[i] = coord_type(0);
	}

	/// @brief explicit cast operator
	///
	/// This enables conversion to a different point type, but
	/// requires explicit programmer specification of such.
	template <typename U>
	explicit inline operator point<U, N>() const { return point<U, N>( *this ); }

	coord_type &operator[]( size_t i ) { return _v[i]; }
	constexpr inline const coord_type &operator[]( size_t i ) const { return _v[i]; }

	coord_type &at( size_t i ) { precondition( i < N, "invalid offset {0}", i ); return _v[i]; }
	const coord_type &at( size_t i ) const { precondition( i < N, "invalid offset {0}", i ); return _v[i]; }

	inline coord_type *begin( void ) { return _v; }
	constexpr inline const coord_type *begin( void ) const { return _v; }
	inline coord_type *end( void ) { return _v + N; }
	constexpr inline const coord_type *end( void ) const { return _v + N; }

	/// @brief Set the coordinates of the point.
	template <typename ... Args>
	void set( Args && ... v )
	{
		static_assert( sizeof...(Args) == N, "expect to initialize point with correct number of coords" );
		setN( 0, std::forward<Args>( v )... );
	}

	/// @brief Move the point by (dx,dy).
	template <typename ... Args>
	void move_by( Args && ... v )
	{
		static_assert( sizeof...(Args) == N, "expect to initialize point with correct number of coords" );
		const point tmp = { std::forward<Args>( v )... };
		*this += tmp;
	}

	/// @brief Add the coordinates of this and p.
	inline point &operator+=( const point &p )
	{
		for ( size_t i = 0; i != N; ++i )
			_v[i] += p[i];
		return *this;
	}
	/// @brief Add the coordinates of this and p.
	point operator+( const point &p ) const
	{
		point r = *this;
		r += p;
		return r;
	}

	/// @brief Subtract the coordinates of p from this.
	inline point &operator-=( const point &p )
	{
		for ( size_t i = 0; i != N; ++i )
			_v[i] -= p[i];
		return *this;
	}
	/// @brief Subtract the coordinates of p from this.
	point operator-( const point &p ) const
	{
		point r = *this;
		r -= p;
		return r;
	}

	/// @brief Scale point by v.
	template <typename U>
	inline point &operator*=( U && v )
	{
		coord_type m( std::forward<U>( v ) );
		for ( size_t i = 0; i != N; ++i )
			_v[i] *= m;
		return *this;
	}
	/// @brief Scale point by v.
	template <typename U>
	point operator*( U && v ) const
	{
		point r = *this;
		r *= std::forward<U>( v );
		return r;
	}

	/// @brief Dot product.
	constexpr coord_type operator*( const point &p ) const
	{
		coord_type v = coord_type(0);
		for ( size_t i = 0; i != N; ++i )
			v += _v[i] * p[i];
		return v;
	}

	inline bool operator<( const point &o ) const
	{
		for ( size_t i = 0; i != N; ++i )
		{
			if ( _v[i] < o[i] )
				return true;
			if ( ! std::equal_to<coord_type>( _v[i], o[i] ) )
				return false;
		}
		return false;
	}
private:
	template <typename A>
	inline void setN( size_t i, A && v )
	{
		_v[i] = std::forward<A>( v );
	}
	template <typename A, typename... Args>
	inline void setN( size_t i, A && v, Args && ... a )
	{
		_v[i] = std::forward<A>( v );
		setN( i + 1, std::forward<Args>( a )... );
	}
	coord_type _v[N];
};

////////////////////////////////////////

/// @brief Output operator for point.
template <typename T, size_t N>
inline std::ostream &operator<<( std::ostream &out, const point<T, N> &p )
{
	out << p[0];
	for ( size_t i = 1; i != N; ++i )
		out << ',' << p[i];
	return out;
}

////////////////////////////////////////

/// @brief Scale point p by v.
template <typename F, typename T, size_t N>
inline point<T, N> operator*( F v, const point<T, N> &p )
{
	return p * static_cast<typename point<T, N>::coord_type>( v );
}

////////////////////////////////////////

template <typename T, size_t N>
inline std::enable_if_t<std::is_floating_point<T>::value, point<T, N>>
round( const point<T, N> &p )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::round( p[i] );
	return r;
}

template <typename T, size_t N>
inline std::enable_if_t<std::is_integral<T>::value, point<T, N>>
round( const point<T, N> &p )
{
	return p;
}

template <typename T, size_t N>
inline std::enable_if_t<! std::is_floating_point<T>::value &&
						! std::is_integral<T>::value, point<T, N>>
round( const point<T, N> &p )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = static_cast<T>( std::round( p[i] ) );
	return r;
}

template <typename T, size_t N>
inline point<long, N> lround( const point<T, N> &p )
{
	point<long, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::lround( p[i] );
	return r;
}

template <typename T, size_t N>
inline point<long long, N> llround( const point<T, N> &p )
{
	point<long, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::llround( p[i] );
	return r;
}

template <typename T, size_t N>
inline point<T, N> abs( const point<T, N> &a )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::abs( a[i] );
	return r;
}

template <typename T, size_t N>
inline point<T, N> ceil( const point<T, N> &a )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::ceil( a[i] );
	return r;
}

template <typename T, size_t N>
inline point<T, N> floor( const point<T, N> &a )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::floor( a[i] );
	return r;
}

template <typename T, size_t N>
inline point<T, N> max( const point<T, N> &a, const point<T, N> &b )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::max( a[i], b[i] );
	return r;
}

template <typename T, size_t N>
inline point<T, N> min( const point<T, N> &a, const point<T, N> &b )
{
	point<T, N> r;
	for ( size_t i = 0; i != N; ++i )
		r[i] = std::min( a[i], b[i] );
	return r;
}

/// @brief Calculate the distance squared between points p1 and p2.
template <typename T, size_t N>
inline T distance_squared( const point<T, N> &p1, const point<T, N> &p2 )
{
	//is_signed can't be overloaded to add alternate types...
	//static_assert( std::is_signed<T>::value, "point coordinate type should be a signed value" );
	T sum = T(0);
	for ( size_t i = 0; i != N; ++i )
	{
		T d = p1[i] - p2[i];
		sum += d * d;
	}
	return sum;
}

/// @brief Calculate the distance between points p1 and p2.
template <typename T, size_t N>
inline typename std::conditional<std::is_floating_point<T>::value || !std::is_integral<T>::value, T, double>::type
distance( const point<T, N> &p1, const point<T, N> &p2 )
{
	using result = typename std::conditional<std::is_floating_point<T>::value || !std::is_integral<T>::value, T, double>::type;
	return std::sqrt( static_cast<result>( distance_squared( p1, p2 ) ) );
}

/// @brief Signed area of triangle (p1, p2, p3)
template <typename T, size_t N>
inline T signed_area( const point<T, 2> &p1, const point<T, 2> &p2, const point<T, 2> &p3 )
{
	static_assert( std::is_signed<T>::value, "point coordinate type should be a signed value" );
	T a = p1[0] * ( p2[1] - p3[1] );
	T b = p2[0] * ( p3[1] - p1[1] );
	T c = p3[0] * ( p1[1] - p2[1] );
	return a + b + c;
}

/// @brief Area of triangle (p1, p2, p3)
template <typename T, size_t N>
inline T area( const point<T, 2> &p1, const point<T, 2> &p2, const point<T, 2> &p3 )
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
template <typename T, size_t N>
inline point_orientation turn( const point<T, 2> &p1, const point<T, 2> &p2, const point<T, 2> &p3 )
{
	T a = signed_area( p1, p2, p3 );
	return ( ( a > T(0) ) ? point_orientation::COUNTERCLOCKWISE :
			 ( ( a < T(0) ) ? point_orientation::CLOCKWISE : point_orientation::COLLINEAR ) );
}

/// @brief Is point p inside the triangle (p1,p2,p3)?
template <typename T, size_t N>
inline bool is_inside( const point<T, 2> &p, const point<T, 2> &p1, const point<T, 2> &p2, const point<T, 2> &p3 )
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
inline point<T, 2> polar( F1 r, F2 a )
{
	static_assert( std::is_floating_point<F2>::value, "polar requires floating point type" );
	return point<T, 2>( static_cast<T>( r * std::cos( a ) ),
					    static_cast<T>( r * std::sin( a ) ) );
}

////////////////////////////////////////

using fpoint = point<float, 2>;
using dpoint = point<double, 2>;
using ipoint = point<int32_t, 2>;
using lpoint = point<int64_t, 2>;

////////////////////////////////////////

} // namespace base


// provide std specialization of the functions above
namespace std
{

template <typename T, size_t N>
inline base::point<T, N> round( const base::point<T, N> &a )
{
	return base::round( a );
}

template <typename T, size_t N>
inline base::point<long, N> lround( const base::point<T, N> &a )
{
	return base::lround( a );
}

template <typename T, size_t N>
inline base::point<long long, N> llround( const base::point<T, N> &a )
{
	return base::llround( a );
}

template <typename T, size_t N>
inline base::point<T, N> abs( const base::point<T, N> &a )
{
	return base::abs( a );
}

template <typename T, size_t N>
inline base::point<T, N> ceil( const base::point<T, N> &a )
{
	return base::ceil( a );
}

template <typename T, size_t N>
inline base::point<T, N> floor( const base::point<T, N> &a )
{
	return base::floor( a );
}

} // namespace std
