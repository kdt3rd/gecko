// Copyright (c) 2014-2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include "type_util.h"

namespace base
{

////////////////////////////////////////

/// @brief Width and height
template <typename T>
class size
{
public:
	static_assert( has_arithmetic_ops<T>::value, "size should be composed of an arithmetic value type" );
	using coord_type = T;
	
	constexpr size( void ) = default;
	constexpr size( const size & ) = default;
	constexpr size( size && ) noexcept = default;
	size &operator=( const size & ) = default;
	size &operator=( size && ) noexcept = default;
	~size( void ) = default;

	/// @brief Constructor with width and height
	constexpr size( coord_type ww, coord_type hh )
		: _w( ww ), _h( hh )
	{
	}

	/// @brief construct a point with a differently typed point.
	///
	/// Requires explicit construction to avoid blind conversion
	template <typename U>
	explicit constexpr size( const size<U> &o )
		: _w( static_cast<coord_type>( o.w() ) ),
		  _h( static_cast<coord_type>( o.h() ) )
	{}

	/// @brief explicit cast operator
	///
	/// This enables conversion to a different size type, but
	/// requires explicit programmer specification of such.
	template <typename U>
	explicit inline operator size<U>() const { return size<U>( *this ); }

	/// @brief Width
	constexpr coord_type w( void ) const { return _w; }

	/// @brief Height
	constexpr coord_type h( void ) const { return _h; }

	/// @brief Set the width and height
	void set( coord_type ww, coord_type hh )
	{
		_w = ww;
		_h = hh;
	}

	/// @brief Set the width
	void set_width( coord_type ww ) { _w = ww; }

	/// @brief Set the height
	void set_height( coord_type hh ) { _h = hh; }

	/// @brief Shrink width and height
	void shrink( coord_type dw, coord_type dh ) { _w -= dw; _h -= dh; }

	/// @brief Grow width and height
	void grow( coord_type dw, coord_type dh ) { _w += dw; _h += dh; }

	/// @brief Add two sizes together
	size operator+( const size &s ) const
	{
		return { _w + s._w, _h + s._h };
	}

	/// @brief less than operator
	inline bool operator<( const size &o ) const
	{
		return _w < o._w || ( std::equal_to<coord_type>()( _w, o._w ) && _h < o._h );
	}

	inline bool empty( void ) const
	{
		return _w <= coord_type( 0 ) && _h <= coord_type( 0 );
	}

private:
	coord_type _w = coord_type( 0 ), _h = coord_type( 0 );
};

////////////////////////////////////////

template <typename T>
inline bool operator==( const size<T> &a, const size<T> &b )
{
	return std::equal_to<T>()( a.w(), b.w() ) && std::equal_to<T>()( a.h(), b.h() );
}

template <typename T>
inline bool operator!=( const size<T> &a, const size<T> &b )
{
	return !( a == b );
}

/// @brief Round size up following rounding rules
template <typename T>
inline size<T> abs( const size<T> &s )
{
	using namespace std;
	return size<T>( static_cast<T>( abs( s.w() ) ), abs( s.h() ) );
}

/// @brief Round size up following rounding rules
template <typename T>
inline size<T> round( const size<T> &s )
{
	using namespace std;
	return size<T>( static_cast<T>( round( s.w() ) ), round( s.h() ) );
}

/// @brief Round size up to nearest integer based on half-way
template <typename T>
inline size<long> lround( const size<T> &s )
{
	using namespace std;
	return size<long>( lround( s.w() ), lround( s.h() ) );
}

/// @brief Round size up to nearest integer based on half-way
template <typename T>
inline size<long long> llround( const size<T> &s )
{
	using namespace std;
	return size<long long>( llround( s.w() ), llround( s.h() ) );
}

/// @brief Round size up to nearest integer
template <typename T>
inline size<T> ceil( const size<T> &s )
{
	using namespace std;
	return size<T>( static_cast<T>( ceil( s.w() ) ), static_cast<T>( ceil( s.h() ) ) );
}

/// @brief Round size up to nearest integer
template <typename T>
inline size<T> floor( const size<T> &s )
{
	using namespace std;
	return size<T>( static_cast<T>( floor( s.w() ) ), static_cast<T>( floor( s.h() ) ) );
}

/// @brief Round size up to nearest integer
template <typename T>
inline size<T> max( const size<T> &a, const size<T> &b )
{
	using namespace std;
	return size<T>( max( a.w(), b.w() ), max( a.h(), b.h() ) );
}

/// @brief Round size up to nearest integer
template <typename T>
inline size<T> min( const size<T> &a, const size<T> &b )
{
	using namespace std;
	return size<T>( min( a.w(), b.w() ), min( a.h(), b.h() ) );
}

////////////////////////////////////////

/// @brief Stream out a size object
template <typename T>
inline std::ostream &operator<<( std::ostream &out, const size<T> &s )
{
	out << s.w() << 'x' << s.h();
	return out;
}

using fsize = size<float>;
using dsize = size<double>;
using isize = size<int32_t>;
using lsize = size<int64_t>;

////////////////////////////////////////

}

// provide std specialization of the functions above
namespace std
{

template <typename T>
inline base::size<T> abs( const base::size<T> &a )
{
	return base::abs( a );
}

template <typename T>
inline base::size<T> round( const base::size<T> &a )
{
	return base::round( a );
}

template <typename T>
inline base::size<long> lround( const base::size<T> &a )
{
	return base::lround( a );
}

template <typename T>
inline base::size<long long> llround( const base::size<T> &a )
{
	return base::llround( a );
}

template <typename T>
inline base::size<T> ceil( const base::size<T> &a )
{
	return base::ceil( a );
}

template <typename T>
inline base::size<T> floor( const base::size<T> &a )
{
	return base::floor( a );
}

template <typename T>
inline base::size<T> max( const base::size<T> &a, const base::size<T> &b )
{
	return base::max( a, b );
}

template <typename T>
inline base::size<T> min( const base::size<T> &a, const base::size<T> &b )
{
	return base::min( a, b );
}

} // namespace std


