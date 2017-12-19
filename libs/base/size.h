//
// Copyright (c) 2014-2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <iostream>
#include <cmath>
#include <cstdint>
#include <functional>
#include <algorithm>

namespace base
{

////////////////////////////////////////

/// @brief Width and height
template <typename T>
class size
{
public:
	static_assert( std::is_arithmetic<T>::value, "size should be a signed value" );
	typedef T coord_type;
	
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
	bool operator<( const size &o ) const
	{
		return _w < o._w || ( std::equal_to<coord_type>()( _w, o._w ) && _h < o._h );
	}

	/// @brief Round size up to nearest integer based on half-way
	void round( void )
	{
		_w = std::lround( _w );
		_h = std::lround( _h );
	}

	/// @brief Round size up to nearest integer
	void ceil( void )
	{
		_w = std::ceil( _w );
		_h = std::ceil( _h );
	}

	/// @brief Round size down to nearest integer
	void floor( void )
	{
		_w = std::floor( _w );
		_h = std::floor( _h );
	}

	/// @brief Maximum operation.
	void maximum( const size &o )
	{
		_w = std::max( _w, o.w() );
		_h = std::max( _h, o.h() );
	}

	/// @brief Minimum operation.
	void minimum( const size &o )
	{
		_w = std::min( _w, o.w() );
		_h = std::min( _h, o.h() );
	}

	bool empty( void ) const
	{
		return _w <= coord_type( 0 ) && _h <= coord_type( 0 );
	}

private:
	coord_type _w = coord_type( 0 ), _h = coord_type( 0 );
};

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

