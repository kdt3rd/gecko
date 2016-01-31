
#include "math_functions.h"
#include <cstdlib>

namespace base
{

////////////////////////////////////////

int64_t math::gcd( int64_t a, int64_t b )
{
	a = std::abs( a );
	b = std::abs( b );

	int64_t t;
	while ( b > 0 )
	{
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

int64_t math::lcm( int64_t a, int64_t b )
{
	return std::abs( a * b ) / gcd( a, b );
}


////////////////////////////////////////

}

