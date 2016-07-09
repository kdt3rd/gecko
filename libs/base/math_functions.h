
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

template <typename F>
inline F lerp( F a, F b, F perc )
{
	// a at 0, b at 1
//	return a * ( 1.F - perc ) + b * perc;
	return a + perc * ( b - a );
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
	
