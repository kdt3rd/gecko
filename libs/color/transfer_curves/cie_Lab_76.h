//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cmath>
#include <sstream>
#include <string>

////////////////////////////////////////

namespace color
{

template <typename T>
struct cie_Lab_76
{
	// TODO: make sure this allows SSE values
	static_assert( std::is_floating_point<T>::value, "Expecting a floating point type" );

	// CIE standard had 0.008856 but ratio lines up discontinuity
	static constexpr T khat = T(216.0/24389.0);
	// CIE standard had 903.3 but ratio lines up discontinuity
	static constexpr T ehat = T(24389.0/27.0);

	inline std::string to_linear_glsl( const std::string &funcName )
	{
		// TODO: uniforms and parameters?
		std::stringstream func;
		func <<
			"float " << funcName << "(float v)\n"
			"{\n"
			"    return (fabs( v * v * v ) > (24389.0f / 27.0f)) ? v * v * v : copysign( ( 116.0f * fabs( v ) - 16.0f ) * 24389.f / 216.f, v );\n"
			"}\n";
		return func.str();
	}		

	inline std::string from_linear_glsl( const std::string &funcName )
	{
		// TODO: uniforms and parameters?
		std::stringstream func;
		func <<
			"float " << funcName << "(float v)\n"
			"{\n"
			"    float x = fabs( v );\n"
			"    x = (x <= (24389.0f/27.0f)) ? ((216.0f * x / 24389.0f + 16.0f) / 116.0f) : cbrt( x );\n"
			"    return copysign( x, v );\n"
			"}\n";
		return func.str();
	}		

	constexpr inline T to_linear( const T v ) const
	{
		using namespace std;
		return abs( v * v * v ) > ehat ? v * v * v : copysign( ( T(116) * abs( v ) - T(16) ) / khat, v );
	}

	constexpr inline T from_linear( const T v ) const
	{
		using namespace std;
		return copysign( (abs( v ) <= ehat) ? ((khat * abs( v ) + T(16)) / T(116)) : cbrt( abs( v ) ), v );
	}

	static constexpr inline T linearize( const T v )
	{
		return cie_Lab_76().to_linear( v );
	}

	static constexpr inline T encode( const T v )
	{
		return cie_Lab_76().from_linear( v );
	}
};

} // namespace color


