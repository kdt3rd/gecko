// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "triplet.h"

#include <algorithm>
#include <base/likely.h>
#include <base/math_functions.h>
#include <iostream>
#include <limits>
#include <type_traits>

////////////////////////////////////////

namespace color
{
///
/// @brief Class coord provides a simple wrapper around an x,y chromaticity coordinate
///
template <typename V> class chroma_coord
{
    static_assert(
        std::is_floating_point<V>::value,
        "color::chroma_coord requires floating point type" );
    static_assert(
        std::is_nothrow_default_constructible<V>::value &&
            std::is_nothrow_copy_constructible<V>::value &&
            std::is_nothrow_move_constructible<V>::value,
        "color::chroma_coord requires a value type to be exception safe" );

public:
    typedef V value_type;

    constexpr chroma_coord( void ) noexcept                 = default;
    constexpr chroma_coord( const chroma_coord & ) noexcept = default;
    chroma_coord &operator=( const chroma_coord & ) noexcept = default;
    constexpr chroma_coord( chroma_coord && ) noexcept       = default;
    chroma_coord &operator=( chroma_coord && ) noexcept = default;
    ~chroma_coord( void )                               = default;

    inline constexpr chroma_coord( value_type xv, value_type yv ) noexcept
        : x( xv ), y( yv )
    {}

    template <typename OV>
    inline constexpr chroma_coord( const chroma_coord<OV> &c ) noexcept
        : x( static_cast<value_type>( c.x ) )
        , y( static_cast<value_type>( c.y ) )
    {}

    template <typename OV>
    chroma_coord &operator=( const chroma_coord<OV> &c ) noexcept
    {
        x = c.x;
        y = c.y;
        return *this;
    }

    inline constexpr triplet<value_type>
    toXYZ( value_type Y = value_type( 1 ) ) const noexcept
    {
        return ( GK_LIKELY( std::abs( y ) > value_type( 0 ) ) )
                   ? triplet<value_type>(
                         Y * x / y, Y, Y * ( value_type( 1 ) - x - y ) / y )
                   : triplet<value_type>(
                         Y * x, Y, Y * ( value_type( 1 ) - x ) );
    }

    static inline constexpr chroma_coord
    fromXYZ( value_type X, value_type Y, value_type Z ) noexcept
    {
        return ( GK_LIKELY(
                   std::abs( X ) > 0.F || std::abs( Y ) > 0.F ||
                   std::abs( Z ) > 0.F ) )
                   ? chroma_coord( X / ( X + Y + Z ), Y / ( X + Y + Z ) )
                   : chroma_coord();
    }
    static inline constexpr chroma_coord
    fromXYZ( const triplet<value_type> &xyz ) noexcept
    {
        return fromXYZ( xyz.x, xyz.y, xyz.z );
    }

    value_type x = value_type( 0 ), y = value_type( 0 );
};

////////////////////////////////////////

template <typename V>
inline constexpr bool
operator==( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
    return base::equal( a.x, b.x ) && base::equal( a.y, b.y );
}

template <typename V>
inline constexpr bool
operator!=( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
    return !( a == b );
}

////////////////////////////////////////

template <typename V>
inline constexpr bool
operator<( const chroma_coord<V> &a, const chroma_coord<V> &b ) noexcept
{
    return ( ( a.x < b.x ) ? true : ( ( b.x < a.x ) ? false : a.y < b.y ) );
}

template <typename V>
inline std::ostream &operator<<( std::ostream &os, const chroma_coord<V> &c )
{
    os << '{' << c.x << ',' << c.y << '}';
    return os;
}

} // namespace color
