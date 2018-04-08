//
// Copyright (c) 2017 Kimball Thurston
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

// found on the openexr mailing list from Ken McGaugh, but attributed to Josh Pines
//
// converted here to linearize from input in 0 - 1, encode to 0 - 1+ using bitScale instead of
// the original hard-coded 1023 / 10-bit thing
template <typename T>
struct mid_gray_log
{
	// TODO: make sure this allows SSE values
	static_assert( std::is_floating_point<T>::value, "Expecting a floating point type" );

	constexpr mid_gray_log( const T linRef = T(0.18), const T logRef = T(445), const T negGam = T(0.6), const T densPerCode = T(0.002), const T bitScale = T(1023) ) noexcept
		: _lin_ref( linRef ), _one_lin_ref( T(1) / linRef ), _log_ref( logRef ),
		  _lin_s( densPerCode / negGam ), _log_s( negGam / densPerCode ),
		  _bits( bitScale ), _bitn( T(1) / bitScale )
	{}

	inline std::string to_linear_glsl( const std::string &funcName )
	{
		// TODO: uniforms and parameters?
		std::stringstream func;
		func << "float " << funcName << "(float v)\n{\n"
			 << "    return " << _lin_ref << " * pow( 10.F, ( min( max( 0, v * " << _bits << " ), " << _bits << " ) - " << _log_ref << " ) * " << _lin_s << " );\n}\n";
		return func.str();
	}		

	inline std::string from_linear_glsl( const std::string &funcName )
	{
		// TODO: uniforms and parameters?
		std::stringstream func;
		func << "float " << funcName << "(float v)\n{\n"
			 << "    return min( " << _bits << ", " << _log_ref << " + log10( max( v, float(1e-10) ) * " << _one_lin_ref << " ) * " << _log_s << " ) * " << _bitn << ";\n}\n";
		return func.str();
	}		

	constexpr inline T to_linear( const T v ) const
	{
		using namespace std;
		return _lin_ref * pow( T(10.0), ( min( max( 0, v * _bits ), _bits ) - _log_ref ) * _lin_s );
	}

	constexpr inline T from_linear( const T v ) const
	{
		using namespace std;
		return min( _bits, _log_ref + log10( max( v, T(1e-10) ) * _one_lin_ref ) * _log_s ) * _bitn;
	}

	const T _lin_ref;
	const T _one_lin_ref;
	const T _log_ref;
	const T _lin_s;
	const T _log_s;
	const T _bits;
	const T _bitn;

	static constexpr inline T linearize( const T v, const T linRef = T(0.18), const T logRef = T(445), const T negGam = T(0.6), const T densPerCode = T(0.002), const T bitScale = T(1023) )
	{
		return mid_gray_log( linRef, logRef, negGam, densPerCode, bitScale ).to_linear( v );
	}

	static constexpr inline T encode( const T v, const T linRef = T(0.18), const T logRef = T(445), const T negGam = T(0.6), const T densPerCode = T(0.002), const T bitScale = T(1023) )
	{
		return mid_gray_log( linRef, logRef, negGam, densPerCode, bitScale ).from_linear( v );
	}
};

} // namespace color



