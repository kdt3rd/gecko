
#include "math_functions.h"

namespace base
{

////////////////////////////////////////

int64_t math::gcd( int64_t a, int64_t b )
{
	int64_t t;
	while ( b > 0 )
	{
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

////////////////////////////////////////

}

