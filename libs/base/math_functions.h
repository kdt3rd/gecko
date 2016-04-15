
#pragma once

#include <cstdint>

namespace base
{

////////////////////////////////////////

int64_t gcd( int64_t a, int64_t b );
int64_t lcm( int64_t a, int64_t b );

template<typename F>
constexpr F degree( F x )
{
	return static_cast<F>( x * F(3.14159265358979323846 / 180.0) );
}

////////////////////////////////////////

}
