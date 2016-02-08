
#pragma once

#include <cstdint>

namespace base
{

////////////////////////////////////////

namespace math
{
	constexpr double PI = 3.14159265358979323846;
	constexpr double RAD2DEG = 180.0 / PI;
	constexpr double DEG2RAD = PI / 180.0;

	int64_t gcd( int64_t a, int64_t b );
	int64_t lcm( int64_t a, int64_t b );

	inline constexpr long double operator"" _deg( long double d )
	{
		return d * DEG2RAD;
	}

	inline constexpr long double operator"" _deg( unsigned long long d )
	{
		return double(d) * DEG2RAD;
	}
}

////////////////////////////////////////

}
