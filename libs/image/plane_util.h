//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once
#include "plane.h"
#include <base/math_functions.h>
#include <algorithm>

////////////////////////////////////////

namespace image
{

template <typename P>
inline typename P::value_type get( const P &p, int x, int y )
{
	return p.get( x, y );
}

template <typename P>
inline typename P::value_type get_zero( const P &p, int x, int y )
{
	if ( x < 0 || x >= p.width() )
		return 0.F;
	if ( y < 0 || y >= p.height() )
		return 0.F;
	return p.get( x, y );
}

template <typename P>
inline typename P::value_type get_hold( const P &p, int x, int y )
{
	return p.get( std::max( 0, std::min( p.lastx(), x ) ),
				  std::max( 0, std::min( p.lasty(), y ) ) );
}

template <typename P>
inline typename P::value_type get_mirror( const P &p, int x, int y )
{
	while ( ! p.in_bounds_x( x ) )
	{
		if ( x < 0 )
			x = -x;
		if ( x > p.lastx() )
			x = p.lastx() * 2 - x;
	}
	while ( ! p.in_bounds_y( y ) )
	{
		if ( y < 0 )
			y = -y;
		if ( y > p.lasty() )
			y = p.lasty() * 2 - y;
	}

	return p.get( x, y );
}

/// bilinear sample, treating requests outside as zero
template <typename P>
inline typename P::value_type bilinear_zero( const P &p, typename P::value_type x, typename P::value_type y )
{
	typedef typename P::value_type value_type;

	int ix = static_cast<int>( x );
	if ( x < 0.F )
		--ix;
	value_type percBx = x - static_cast<value_type>( ix );
	int iy = static_cast<int>( y );
	if ( y < 0.F )
		--iy;
	value_type percBy = y - static_cast<value_type>( iy );

	value_type a00 = get_zero( p, ix, iy );
	value_type a10 = get_zero( p, ix + 1, iy );
	value_type a01 = get_zero( p, ix, iy + 1 );
	value_type a11 = get_zero( p, ix + 1, iy + 1 );
	value_type t0 = base::lerp( a00, a10, percBx );
	value_type t1 = base::lerp( a01, a11, percBx );
	return base::lerp( t0, t1, percBy );
}

/// bilinear sample, holding edge
template <typename P>
inline typename P::value_type bilinear_hold( const P &p, typename P::value_type x, typename P::value_type y )
{
	typedef typename P::value_type value_type;

	int ix = static_cast<int>( x );
	if ( x < 0.F )
		--ix;
	value_type percBx = x - static_cast<value_type>( ix );
	int iy = static_cast<int>( y );
	if ( y < 0.F )
		--iy;
	value_type percBy = y - static_cast<value_type>( iy );

	value_type a00 = get_hold( p, ix, iy );
	value_type a10 = get_hold( p, ix + 1, iy );
	value_type a01 = get_hold( p, ix, iy + 1 );
	value_type a11 = get_hold( p, ix + 1, iy + 1 );
	value_type t0 = base::lerp( a00, a10, percBx );
	value_type t1 = base::lerp( a01, a11, percBx );
	return base::lerp( t0, t1, percBy );
}

} // namespace image

