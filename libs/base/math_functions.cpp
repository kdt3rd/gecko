//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "math_functions.h"
#include <cstdlib>

namespace base
{

////////////////////////////////////////

int64_t gcd( int64_t a, int64_t b )
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

int64_t lcm( int64_t a, int64_t b )
{
	return std::abs( a * b ) / gcd( a, b );
}


////////////////////////////////////////

}

