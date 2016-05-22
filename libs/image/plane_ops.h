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
#include "plane.h"
#include "plane_stats.h"

////////////////////////////////////////

namespace image
{

inline plane create_plane( int w, int h, const engine::computed_value<float> &v )
{
	engine::dimensions d;
	d.x = static_cast<uint16_t>( w );
	d.y = static_cast<uint16_t>( h );
	return plane( "assign_plane", d, v );
}

/// unary plane operators
inline plane operator-( const plane &p )
{
	return plane( "mul_planenumber", p.dims(), p, -1.F );
}
inline plane operator-( plane &&p )
{
	return plane( "mul_planenumber", p.dims(), std::move( p ), -1.F );
}

/// @defgroup Add two planes together, or add a value to a plane. We
/// enumerate all the variants explicitly so we can tag r-value
/// objects for re-use, non-cache, etc.
///
/// @{
inline plane operator+( const plane &a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to add planes of different sizes" );
	return plane( "add_planeplane", a.dims(), a, b );
}

inline plane operator+( plane &&a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to add planes of different sizes" );
	return plane( "add_planeplane", a.dims(), std::move( a ), b );
}

inline plane operator+( const plane &a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to add planes of different sizes" );
	return plane( "add_planeplane", a.dims(), a, std::move( b ) );
}

inline plane operator+( plane &&a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to add planes of different sizes" );
	return plane( "add_planeplane", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator+( const plane &p, float v )
{
	return plane( "add_planenumber", p.dims(), p, v );
}
inline plane operator+( plane &&p, float v )
{
	return plane( "add_planenumber", p.dims(), std::move( p ), v );
}
inline plane operator+( float v, const plane &p )
{
	return plane( "add_planenumber", p.dims(), p, v );
}
inline plane operator+( float v, plane &&p )
{
	return plane( "add_planenumber", p.dims(), std::move( p ), v );
}

inline plane &operator+=( plane &a, const plane &b )
{
	a = a + b;
	return a;
}
inline plane &operator+=( plane &a, plane &&b )
{
	a = a + std::move( b );
	return a;
}
inline plane &operator+=( plane &a, float b )
{
	a = a + b;
	return a;
}
/// @}

/// @defgroup subtract planes / values from planes.
/// Enumerate all the variants explicitly so we can tag r-value
/// objects for re-use, non-cache, etc.
///
/// @{
inline plane operator-( const plane &a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to subtract planes of different sizes" );
	return plane( "sub_planeplane", a.dims(), a, b );
}

inline plane operator-( plane &&a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to subtract planes of different sizes" );
	return plane( "sub_planeplane", a.dims(), std::move( a ), b );
}

inline plane operator-( const plane &a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to subtract planes of different sizes" );
	return plane( "sub_planeplane", a.dims(), a, std::move( b ) );
}

inline plane operator-( plane &&a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to subtract planes of different sizes" );
	return plane( "sub_planeplane", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator-( const plane &p, float v )
{
	return plane( "sub_planenumber", p.dims(), p, v );
}
inline plane operator-( plane &&p, float v )
{
	return plane( "add_planenumber", p.dims(), std::move( p ), - v );
}
inline plane operator-( float v, const plane &p )
{
	return plane( "muladd_planenumbernumber", p.dims(), p, -1.F, v );
}
inline plane operator-( float v, plane &&p )
{
	return plane( "muladd_planenumbernumber", p.dims(), std::move( p ), -1.F, v );
}

inline plane &operator-=( plane &a, const plane &b )
{
	a = a - b;
	return a;
}
inline plane &operator-=( plane &a, plane &&b )
{
	a = a - std::move( b );
	return a;
}
inline plane &operator-=( plane &a, float b )
{
	a = a - b;
	return a;
}
/// @}

/// @defgroup Multiplies planes and planes/values.  Enumerate all the
/// variants explicitly so we can tag r-value objects for re-use,
/// non-cache, etc.
///
/// @{
inline plane operator*( const plane &a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to multiply planes of different sizes" );
	return plane( "mul_planeplane", a.dims(), a, b );
}

inline plane operator*( plane &&a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to multiply planes of different sizes" );
	return plane( "mul_planeplane", a.dims(), std::move( a ), b );
}

inline plane operator*( const plane &a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to multiply planes of different sizes" );
	return plane( "mul_planeplane", a.dims(), a, std::move( b ) );
}

inline plane operator*( plane &&a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to multiply planes of different sizes" );
	return plane( "mul_planeplane", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator*( const plane &p, float v )
{
	return plane( "mul_planenumber", p.dims(), p, v );
}
inline plane operator*( plane &&p, float v )
{
	return plane( "mul_planenumber", p.dims(), std::move( p ), v );
}
inline plane operator*( float v, const plane &p )
{
	return plane( "mul_planenumber", p.dims(), p, v );
}
inline plane operator*( float v, plane &&p )
{
	return plane( "mul_planenumber", p.dims(), std::move( p ), v );
}

inline plane &operator*=( plane &a, const plane &b )
{
	a = a * b;
	return a;
}
inline plane &operator*=( plane &a, plane &&b )
{
	a = a * std::move( b );
	return a;
}
inline plane &operator*=( plane &a, float b )
{
	a = a * b;
	return a;
}
/// @}


/// @defgroup Divides planes and planes/values.  Enumerate all the
/// variants explicitly so we can tag r-value objects for re-use,
/// non-cache, etc.
///
/// @{
inline plane operator/( const plane &a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to divide planes of different sizes" );
	return plane( "div_planeplane", a.dims(), a, b );
}

inline plane operator/( plane &&a, const plane &b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to divide planes of different sizes" );
	return plane( "div_planeplane", a.dims(), std::move( a ), b );
}

inline plane operator/( const plane &a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to divide planes of different sizes" );
	return plane( "div_planeplane", a.dims(), a, std::move( b ) );
}

inline plane operator/( plane &&a, plane &&b )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "unable to divide planes of different sizes" );
	return plane( "div_planeplane", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator/( const plane &p, float v )
{
	return plane( "mul_planenumber", p.dims(), p, 1.F / v );
}
inline plane operator/( plane &&p, float v )
{
	return plane( "mul_planenumber", p.dims(), std::move( p ), 1.F / v );
}
inline plane operator/( float v, const plane &p )
{
	return plane( "div_numberplane", p.dims(), v, p );
}
inline plane operator/( float v, plane &&p )
{
	return plane( "div_numberplane", p.dims(), v, std::move( p ) );
}

inline plane &operator/=( plane &a, const plane &b )
{
	a = a / b;
	return a;
}
inline plane &operator/=( plane &a, plane &&b )
{
	a = a / std::move( b );
	return a;
}
inline plane &operator/=( plane &a, float b )
{
	a = a / b;
	return a;
}
/// @}

} // namespace image



