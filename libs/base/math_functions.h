
#pragma once

#include <cstdint>

namespace base
{

////////////////////////////////////////

namespace math
{
	struct FloatingPoint
	{
		constexpr FloatingPoint( long double xx )
			: x( xx )
		{
		}

		constexpr operator float() const
		{
			return static_cast<float>( x );
		}

		constexpr operator double() const
		{
			return static_cast<double>( x );
		}

		constexpr operator long double() const
		{
			return x;
		}

		long double x;
	};

	constexpr double PI = 3.14159265358979323846;
	constexpr double RAD2DEG = 180.0 / PI;
	constexpr double DEG2RAD = PI / 180.0;

	int64_t gcd( int64_t a, int64_t b );
	int64_t lcm( int64_t a, int64_t b );

	inline constexpr FloatingPoint operator"" _deg( long double d )
	{
		return FloatingPoint( d * DEG2RAD );
	}

	inline constexpr long double operator"" _deg( unsigned long long d )
	{
		return double(d) * DEG2RAD;
	}
}

////////////////////////////////////////

}
