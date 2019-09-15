// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/math_functions.h>
#include <type_traits>

////////////////////////////////////////

namespace color
{
///
/// @brief Class triplet provides a simple collection of 3 values
///
template <typename V> class triplet
{
    static_assert(
        std::is_floating_point<V>::value,
        "color::triplet requires floating point type" );

public:
    typedef V value_type;

    inline constexpr triplet( void ) noexcept
        : x( value_type( 0 ) ), y( value_type( 0 ) ), z( value_type( 0 ) )
    {}
    inline constexpr triplet( value_type v ) : x( v ), y( v ), z( v ) {}
    inline constexpr triplet( value_type a, value_type b, value_type c )
        : x( a ), y( b ), z( c )
    {}

    value_type x, y, z;
};

////////////////////////////////////////

template <typename V>
inline constexpr bool
operator==( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return base::equal( a.x, b.x ) && base::equal( a.y, b.y ) &&
           base::equal( a.z, b.z );
}

template <typename V>
inline constexpr bool
operator!=( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return !( a == b );
}

template <typename V>
inline constexpr triplet<V>
operator+( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return triplet<V>( a.x + b.x, a.y + b.y, a.z + a.z );
}

template <typename V>
inline constexpr triplet<V>
operator-( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return triplet<V>( a.x - b.x, a.y - b.y, a.z - a.z );
}

template <typename V>
inline constexpr triplet<V>
operator*( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return triplet<V>( a.x * b.x, a.y * b.y, a.z * a.z );
}

template <typename V>
inline constexpr triplet<V>
operator/( const triplet<V> &a, const triplet<V> &b ) noexcept
{
    return triplet<V>( a.x / b.x, a.y / b.y, a.z / a.z );
}

} // namespace color
