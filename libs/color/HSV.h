// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <base/math_functions.h>

////////////////////////////////////////

namespace color
{

template <typename V>
struct HSV
{
	typedef V value_type;

	/// Computes HSV using the hexagonal approximation of hue
	inline void to_hex( value_type &h, value_type &s, value_type &v, value_type r, value_type g, value_type b )
	{
		if ( r >= g && r >= b )
		{
			value_type m = b;
			// R is max
			// check green is min
			if ( b > g )
				m = g;

			v = r;
			if ( ( r - m ) < std::numeric_limits<value_type>::epsilon() )
			{
				h = 0.0;
				s = 0.0;
			}
			else
			{
				h = fmod( ( g - b ) / ( r - m ), 6.0 ) * 60.0;
				s = ( r - m ) / r;
			}
		}
		else if ( g >= r && g >= b )
		{
			value_type m = b;
			// G is max
			// check if red is min instead of blue
			if ( b > r )
				m = r;

			v = g;
			if ( ( g - m ) < std::numeric_limits<value_type>::epsilon() )
			{
				h = 0.0;
				s = 0.0;
			}
			else
			{
				h = ( ( b - r ) / ( g - m ) + 2.0 ) * 60.0;
				s = ( g - m ) / g;
			}
		}

		// left with case of blue is max
		value_type m = r;
		// check if green is min instead of red
		if ( r > g )
			m = g;

		v = b;
		if ( ( b - m ) < std::numeric_limits<value_type>::epsilon() )
		{
			h = 0.0;
			s = 0.0;
		}
		else
		{
			h = ( ( r - g ) / ( b - m ) + 4.0 ) * 60.0;
			s = ( b - m ) / b;
		}
	}

	inline void to_cylindrical( value_type &h, value_type &s, value_type &v, value_type r, value_type g, value_type b )
	{
		value_type alpha = (2.0 * r - g - b) / 2.0;
		value_type beta = ( sqrt( 3.0 ) / 2.0 ) * ( g - b );
		v = std::max( r, std::max( g, b ) );
		if ( v < std::numeric_limits<value_type>::epsilon() )
		{
			h = 0.0;
			s = 0.0;
		}
		else
		{
			s = sqrt( alpha * alpha + beta * beta ) / v;
			h = base::rad2deg( atan2( beta, alpha ) );
		}
	}	
};

} // namespace color



