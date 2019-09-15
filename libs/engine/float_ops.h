// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "computed_value.h"

#include <base/contract.h>

////////////////////////////////////////

namespace engine
{
typedef computed_value<float>  cvf;
typedef computed_value<double> cvd;

inline cvd upcast( const cvf &f ) { return cvd( "d.castto", nulldim, f ); }
inline cvf downcast( const cvd &d ) { return cvf( "d.castfrom", nulldim, d ); }

/// unary negate
inline cvf operator-( const cvf &v ) { return cvf( "f.negate", nulldim, v ); }
inline cvd operator-( const cvd &v ) { return cvd( "d.negate", nulldim, v ); }

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
