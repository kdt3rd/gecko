//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <limits>
#include <vector>

namespace base
{

////////////////////////////////////////

// TODO: switch to the std c++ lib when available
int64_t gcd( int64_t a, int64_t b );
int64_t lcm( int64_t a, int64_t b );

////////////////////////////////////////

template <typename F>
inline constexpr F deg2rad( F x ) noexcept
{
	return static_cast<F>( x * F(3.141592653589793238462643383279502884L / 180.0L) );
}

template <typename F>
inline constexpr F rad2deg( F x ) noexcept
{
	return static_cast<F>( x * F(180.0L / 3.141592653589793238462643383279502884L) );
}

////////////////////////////////////////

namespace detail
{

template <typename V, bool> struct comparator_equal;
template <typename V>
struct comparator_equal<V,false>
{
	static inline constexpr bool op( V a, V b, int ) noexcept { return a == b; }
};

template <typename V>
struct comparator_equal<V,true>
{
	static inline constexpr bool op( V a, V b, int ulp ) noexcept
	{
		// the machine epsilon has to be scaled to the magnitude of the values used
		// and multiplied by the desired precision in ULPs (units in the last place)
		return ( ( std::abs( a - b ) < std::numeric_limits<V>::epsilon() * std::abs( a + b ) * ulp )
				 // unless the result is subnormal
				 || std::abs( a - b ) < std::numeric_limits<V>::min() );
	}
};

} // detail

template <typename F>
inline constexpr bool equal( F a, F b, int ulp = 2 ) noexcept
{
	return detail::comparator_equal<F,std::is_floating_point<F>::value>::op( a, b, ulp );
}

////////////////////////////////////////

/// normal linear interpolation: a when perc is 0, b when perc is 1.0
///
/// if a and b are in the same range / order of magnitude, it is safe
/// to use @sa lerp_fast, otherwise, use this function
///
template <typename F>
inline F lerp( F a, F b, F perc )
{
	return a * ( 1.F - perc ) + b * perc;
}

/// @sa lerp
template <typename F>
inline F lerp_fast( F a, F b, F perc )
{
	return a + perc * ( b - a );
}

////////////////////////////////////////

// derived from wikipedia:
// p(t) = p1 + (p2/2 - p0/2)*t + (p0 - 5*p1/2 + 2*p2 - p3/2)*t^2 + (3*p1/2 + p3/2 - p0/2 - 3*p2/2)*t^3
template <typename F>
inline F cubic_interp( F t, F p0, F p1, F p2, F p3 )
{
	return ( p1 + F(0.5) * ( p2 - p0 ) * t +
			 ( p0 - F(2.5) * p1 + (p2 + p2) - F(0.5) * p3 ) * ( t * t ) +
			 ( F(1.5) * ( p1 - p2 ) + F(0.5) * ( p3 - p0 ) ) * ( t * t * t ) );
}

////////////////////////////////////////

inline std::vector<float>
atrous_expand( const std::vector<float> &a )
{
	std::vector<float> ret;
	ret.resize( ( a.size() - 1 ) * 2 + 1 );
	for ( size_t i = 0; i != a.size(); ++i )
	{
		ret[i*2] = a[i];
		if ( i+1 != a.size() )
			ret[i*2+1] = 0.F;
	}
	return ret;
}

inline std::vector<float>
dirac_negate( const std::vector<float> &a )
{
	std::vector<float> ret = a;
	for ( float &v: ret )
		v = -v;
	ret[ret.size()/2] += 1.F;
	return ret;
}

} // namespace base

/// Global literal constant operator for degrees
inline constexpr long double operator "" _deg( long double d ) noexcept
{
	return base::deg2rad( d );
}

inline constexpr float operator "" _degf( long double d ) noexcept
{
	return static_cast<float>( base::deg2rad( d ) );
}

