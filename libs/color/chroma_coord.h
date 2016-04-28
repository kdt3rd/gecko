//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <limits>
#include <algorithm>
#include <base/math_functions.h>
#include "triplet.h"

////////////////////////////////////////

namespace color
{

///
/// @brief Class coord provides a simple wrapper around an x,y chromaticity coordinate
///
template <typename V>
class chroma_coord
{
	static_assert( std::is_floating_point<V>::value, "color::chroma_coord requires floating point type" );
public:
	typedef V value_type;

	inline constexpr chroma_coord( void ) noexcept : x( value_type(0) ), y( value_type(0) ) {}
	inline constexpr chroma_coord( value_type xv, value_type yv ) noexcept : x( xv ), y( yv ) {}

	template <typename OV>
	inline constexpr chroma_coord( const chroma_coord<OV> &c ) noexcept : x( static_cast<value_type>(c.x) ), y( static_cast<value_type>(c.y) ) {}

	chroma_coord( const chroma_coord & ) = default;
	inline constexpr chroma_coord( chroma_coord &&c ) noexcept : x( std::move(c.x) ), y( std::move(c.y) ) {}

	~chroma_coord( void ) = default;
	chroma_coord &operator=( const chroma_coord & ) = default;
	chroma_coord &operator=( chroma_coord && ) = default;
	template <typename OV>
	chroma_coord &operator=( const chroma_coord<OV> &c ) noexcept
	{
		x = c.x;
		y = c.y;
		return *this;
	}

	inline constexpr triplet<value_type> toXYZ( value_type Y = value_type(1) ) const noexcept
	{
		return triplet<value_type>( Y * x / y, Y, Y * ( value_type(1) - x - y ) / y );
	}

	static inline constexpr chroma_coord fromXYZ( value_type X, value_type Y, value_type Z ) noexcept
	{
		return chroma_coord( X / (X + Y + Z), Y / (X + Y + Z) );
	}
	static inline constexpr chroma_coord fromXYZ( const triplet<value_type> &xyz ) noexcept
	{
		return fromXYZ( xyz.x, xyz.y, xyz.z );
	}

	value_type x, y;
};

////////////////////////////////////////

template <typename V>
inline constexpr bool operator==( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
	return base::equal( a.x, b.x ) && base::equal( a.y, b.y );
}

template <typename V>
inline constexpr bool operator!=( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
	return !( a == b );
}

////////////////////////////////////////

template <typename V>
inline constexpr bool operator<( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
	return ( ( a.x < b.x ) ? true : ( ( b.x < a.x ) ? false : a.y < b.y ) );
}

} // namespace color



