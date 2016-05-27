//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <base/contract.h>
#include "computed_value.h"

////////////////////////////////////////

namespace engine
{

typedef computed_value<float> cvf;
typedef computed_value<double> cvd;

inline cvd upcast( const cvf &f )
{
	return cvd( "d.castto", nulldim, f );
}
inline cvf downcast( const cvd &d )
{
	return cvf( "d.castfrom", nulldim, d );
}

/// unary negate
inline cvf operator-( const cvf &v )
{
	return cvf( "f.negate", nulldim, v );
}
inline cvd operator-( const cvd &v )
{
	return cvd( "d.negate", nulldim, v );
}

/// @defgroup binary float operations
/// @{

inline cvf operator+( const cvf &a, const cvf &b )
{
	return cvf( "f.add", nulldim, a, b );
}
inline cvf operator+( const cvf &a, float b )
{
	return cvf( "f.add", nulldim, a, b );
}
inline cvf operator+( float a, const cvf &b )
{
	return cvf( "f.add", nulldim, a, b );
}
inline cvd operator+( double a, const cvf &b )
{
	return cvd( "d.add", nulldim, a, upcast( b ) );
}
inline cvd operator+( const cvf &a, double b )
{
	return cvd( "d.add", nulldim, upcast( a ), b );
}
inline cvd operator+( const cvd &a, const cvd &b )
{
	return cvd( "d.add", nulldim, a, b );
}
inline cvd operator+( const cvd &a, double b )
{
	return cvd( "d.add", nulldim, a, b );
}
inline cvd operator+( double a, const cvd &b )
{
	return cvd( "d.add", nulldim, a, b );
}
inline cvd operator+( const cvd &a, const cvf &b )
{
	return cvd( "d.add", nulldim, a, upcast( b ) );
}
inline cvd operator+( const cvf &a, const cvd &b )
{
	return cvd( "d.add", nulldim, upcast( a ), b );
}

inline cvf operator-( const cvf &a, const cvf &b )
{
	return cvf( "f.sub", nulldim, a, b );
}
inline cvf operator-( const cvf &a, float b )
{
	return cvf( "f.sub", nulldim, a, b );
}
inline cvf operator-( float a, const cvf &b )
{
	return cvf( "f.sub", nulldim, a, b );
}
inline cvd operator-( double a, const cvf &b )
{
	return cvd( "d.sub", nulldim, a, upcast( b ) );
}
inline cvd operator-( const cvf &a, double b )
{
	return cvd( "d.sub", nulldim, upcast( a ), b );
}
inline cvd operator-( const cvd &a, const cvd &b )
{
	return cvd( "d.sub", nulldim, a, b );
}
inline cvd operator-( const cvd &a, double b )
{
	return cvd( "d.sub", nulldim, a, b );
}
inline cvd operator-( double a, const cvd &b )
{
	return cvd( "d.sub", nulldim, a, b );
}
inline cvd operator-( const cvd &a, const cvf &b )
{
	return cvd( "d.sub", nulldim, a, upcast( b ) );
}
inline cvd operator-( const cvf &a, const cvd &b )
{
	return cvd( "d.sub", nulldim, upcast( a ), b );
}

inline cvf operator*( const cvf &a, const cvf &b )
{
	return cvf( "f.mul", nulldim, a, b );
}
inline cvf operator*( const cvf &a, float b )
{
	return cvf( "f.mul", nulldim, a, b );
}
inline cvf operator*( float a, const cvf &b )
{
	return cvf( "f.mul", nulldim, a, b );
}
inline cvd operator*( double a, const cvf &b )
{
	return cvd( "d.mul", nulldim, a, upcast( b ) );
}
inline cvd operator*( const cvf &a, double b )
{
	return cvd( "d.mul", nulldim, upcast( a ), b );
}
inline cvd operator*( const cvd &a, const cvd &b )
{
	return cvd( "d.mul", nulldim, a, b );
}
inline cvd operator*( const cvd &a, double b )
{
	return cvd( "d.mul", nulldim, a, b );
}
inline cvd operator*( double a, const cvd &b )
{
	return cvd( "d.mul", nulldim, a, b );
}
inline cvd operator*( const cvd &a, const cvf &b )
{
	return cvd( "d.mul", nulldim, a, upcast( b ) );
}
inline cvd operator*( const cvf &a, const cvd &b )
{
	return cvd( "d.mul", nulldim, upcast( a ), b );
}

inline cvf operator/( const cvf &a, const cvf &b )
{
	return cvf( "f.div", nulldim, a, b );
}
inline cvf operator/( const cvf &a, float b )
{
	return cvf( "f.div", nulldim, a, b );
}
inline cvf operator/( float a, const cvf &b )
{
	return cvf( "f.div", nulldim, a, b );
}
inline cvd operator/( double a, const cvf &b )
{
	return cvd( "d.div", nulldim, a, upcast( b ) );
}
inline cvd operator/( const cvf &a, double b )
{
	return cvd( "d.div", nulldim, upcast( a ), b );
}
inline cvd operator/( const cvd &a, const cvd &b )
{
	return cvd( "d.div", nulldim, a, b );
}
inline cvd operator/( const cvd &a, double b )
{
	return cvd( "d.div", nulldim, a, b );
}
inline cvd operator/( double a, const cvd &b )
{
	return cvd( "d.div", nulldim, a, b );
}
inline cvd operator/( const cvd &a, const cvf &b )
{
	return cvd( "d.div", nulldim, a, upcast( b ) );
}
inline cvd operator/( const cvf &a, const cvd &b )
{
	return cvd( "d.div", nulldim, upcast( a ), b );
}

/// @}

/// @defgroup binary self-modifying float operations
/// @{

inline cvf &operator+=( cvf &a, const cvf &b )
{
	a = a + b;
	return a;
}
inline cvf &operator+=( cvf &a, float b )
{
	a = a + b;
	return a;
}
inline cvd &operator+=( cvd &a, const cvd &b )
{
	a = a + b;
	return a;
}
inline cvd &operator+=( cvd &a, float b )
{
	a = a + static_cast<double>( b );
	return a;
}
inline cvd &operator+=( cvd &a, double b )
{
	a = a + b;
	return a;
}
inline cvd &operator+=( cvd &a, const cvf &b )
{
	a = a + b;
	return a;
}

inline cvf &operator-=( cvf &a, const cvf &b )
{
	a = a - b;
	return a;
}
inline cvf &operator-=( cvf &a, float b )
{
	a = a - b;
	return a;
}
inline cvd &operator-=( cvd &a, const cvd &b )
{
	a = a - b;
	return a;
}
inline cvd &operator-=( cvd &a, float b )
{
	a = a - static_cast<double>( b );
	return a;
}
inline cvd &operator-=( cvd &a, double b )
{
	a = a - b;
	return a;
}
inline cvd &operator-=( cvd &a, const cvf &b )
{
	a = a - b;
	return a;
}

inline cvf &operator*=( cvf &a, const cvf &b )
{
	a = a * b;
	return a;
}
inline cvf &operator*=( cvf &a, float b )
{
	a = a * b;
	return a;
}
inline cvd &operator*=( cvd &a, const cvd &b )
{
	a = a * b;
	return a;
}
inline cvd &operator*=( cvd &a, float b )
{
	a = a * static_cast<double>( b );
	return a;
}
inline cvd &operator*=( cvd &a, double b )
{
	a = a * b;
	return a;
}
inline cvd &operator*=( cvd &a, const cvf &b )
{
	a = a * b;
	return a;
}

inline cvf &operator/=( cvf &a, const cvf &b )
{
	a = a / b;
	return a;
}
inline cvf &operator/=( cvf &a, float b )
{
	a = a / b;
	return a;
}
inline cvd &operator/=( cvd &a, const cvd &b )
{
	a = a / b;
	return a;
}
inline cvd &operator/=( cvd &a, float b )
{
	a = a / static_cast<double>( b );
	return a;
}
inline cvd &operator/=( cvd &a, double b )
{
	a = a / b;
	return a;
}
inline cvd &operator/=( cvd &a, const cvf &b )
{
	a = a / b;
	return a;
}

/// @}

class registry;
void register_float_ops( registry &r );

} // namespace engine



