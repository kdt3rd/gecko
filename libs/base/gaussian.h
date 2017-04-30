//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include <cmath>

////////////////////////////////////////

namespace base
{

template <typename T>
inline T gauss_integral( T a, T b )
{
	T ca = std::min( std::max( T(-1), a ), T(1) );
	T cb = std::min( std::max( T(-1), b ), T(1) );
	double sa = 0.55303064330427195028 * std::erf( ca * 1.17741002251547 );
	double sb = 0.55303064330427195028 * std::erf( cb * 1.17741002251547 );
	return sb - sa;
}

template <typename T>
inline std::vector<T>
create_gaussian( T radius )
{
	size_t kSize = static_cast<size_t>( radius * T(2) + T(1) );
	if ( kSize % 2 == 0 )
		++kSize;
	T initValue = T(-0.5);
	T nextValue = T(0.5);
	std::vector<T> retval( kSize, T(0) );
	size_t halfK = kSize / 2;
	T sum = T(0);
	for ( size_t x = 0; x <= halfK; ++x )
	{
		T partInt = gauss_integral( initValue / radius, nextValue / radius );
		if ( x == 0 )
		{
			sum += partInt;
			retval[halfK] = partInt;
		}
		else
		{
			retval[halfK + x] = partInt;
			retval[halfK - x] = partInt;
			sum += T(2) * partInt;
		}

		initValue = nextValue;
		nextValue = std::min( radius, nextValue + T(1) );
	}
	for ( auto &x: retval )
		x /= sum;

	return retval;
}

} // namespace base

