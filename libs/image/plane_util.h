// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "plane.h"

#include <algorithm>
#include <base/compiler_support.h>
#include <base/math_functions.h>

////////////////////////////////////////

namespace image
{
template <typename P>
GK_FORCE_INLINE typename P::value_type get( const P &p, int x, int y )
{
    return p.get( x, y );
}

template <typename P>
GK_FORCE_INLINE typename P::value_type get_zero( const P &p, int x, int y )
{
    if ( x < p.x1() || x > p.x2() )
        return 0.F;
    if ( y < p.y1() || y > p.y2() )
        return 0.F;
    return p.get( x, y );
}

template <typename P>
GK_FORCE_INLINE typename P::value_type get_hold( const P &p, int x, int y )
{
    return p.get(
        std::max( p.x1(), std::min( p.x2(), x ) ),
        std::max( p.y1(), std::min( p.y2(), y ) ) );
}

template <typename P>
GK_FORCE_INLINE typename P::value_type get_mirror( const P &p, int x, int y )
{
    while ( !p.in_bounds_x( x ) )
    {
        if ( x < p.x1() )
            x += p.x1() - x;
        if ( x > p.x2() )
            x += p.x2() - x;
    }
    while ( !p.in_bounds_y( y ) )
    {
        if ( y < p.y1() )
            y += p.y1() - y;
        if ( y > p.y2() )
            y += p.y2() - y;
    }

    return p.get( x, y );
}

template <typename P> GK_FORCE_INLINE P bilinCoord( int &p1, int &p2, P v )
{
    P perc;
    if ( v < 0.F )
    {
        //		p1 = static_cast<int>( v + P(0.5) );
        p1 = static_cast<int>( v );
        p2 = p1 - 1;
        //		perc = -( v + P(0.5) - static_cast<P>( p1 ) );
        perc = -( v - static_cast<P>( p1 ) );
    }
    else
    {
        //		p1 = static_cast<int>( v - P(0.5) );
        p1 = static_cast<int>( v );
        p2 = p1 + 1;
        //		perc = v - P(0.5) - static_cast<P>( p1 );
        perc = v - static_cast<P>( p1 );
    }
    return perc;
}

/// bilinear sample, treating requests outside as zero
template <typename P>
GK_FORCE_INLINE typename P::value_type
bilinear_zero( const P &p, typename P::value_type x, typename P::value_type y )
{
    typedef typename P::value_type value_type;

    int        ix, ix2;
    value_type percBx = bilinCoord( ix, ix2, x );
    int        iy, iy2;
    value_type percBy = bilinCoord( iy, iy2, y );

    value_type a00 = get_zero( p, ix, iy );
    value_type a10 = get_zero( p, ix2, iy );
    value_type a01 = get_zero( p, ix, iy2 );
    value_type a11 = get_zero( p, ix2, iy2 );
    value_type t0  = base::lerp( a00, a10, percBx );
    value_type t1  = base::lerp( a01, a11, percBx );
    return base::lerp( t0, t1, percBy );
}

/// bilinear sample, holding edge
template <typename P>
GK_FORCE_INLINE typename P::value_type
bilinear_hold( const P &p, typename P::value_type x, typename P::value_type y )
{
    typedef typename P::value_type value_type;

    int        ix, ix2;
    value_type percBx = bilinCoord( ix, ix2, x );
    int        iy, iy2;
    value_type percBy = bilinCoord( iy, iy2, y );

    value_type a00 = get_hold( p, ix, iy );
    value_type a10 = get_hold( p, ix2, iy );
    value_type a01 = get_hold( p, ix, iy2 );
    value_type a11 = get_hold( p, ix2, iy2 );
    value_type t0  = base::lerp( a00, a10, percBx );
    value_type t1  = base::lerp( a01, a11, percBx );
    return base::lerp( t0, t1, percBy );
}

/// bilinear sample, treating requests outside as zero
template <typename P>
GK_FORCE_INLINE typename P::value_type bilinear_bignum(
    const P &              p,
    typename P::value_type x,
    typename P::value_type y,
    typename P::value_type bignum )
{
    typedef typename P::value_type value_type;

    int        ix, ix2;
    value_type percBx = bilinCoord( ix, ix2, x );
    int        iy, iy2;
    value_type percBy = bilinCoord( iy, iy2, y );

    if ( p.in_bounds( ix, iy ) && p.in_bounds( ix2, iy2 ) )
    {
        value_type a00 = get( p, ix, iy );
        value_type a10 = get( p, ix2, iy );
        value_type a01 = get( p, ix, iy2 );
        value_type a11 = get( p, ix2, iy2 );
        value_type t0  = base::lerp( a00, a10, percBx );
        value_type t1  = base::lerp( a01, a11, percBx );
        return base::lerp( t0, t1, percBy );
    }

    return bignum;
}

} // namespace image
