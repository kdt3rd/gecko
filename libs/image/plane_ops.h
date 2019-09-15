// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "optical_flow.h"
#include "plane.h"
#include "plane_convolve.h"
#include "plane_resize.h"
#include "plane_stats.h"
#include "spatial_filter.h"

#include <base/contract.h>
#include <engine/float_ops.h>

////////////////////////////////////////

namespace image
{
plane create_plane( int x1, int y1, int x2, int y2, float v );
plane create_plane(
    int x1, int y1, int x2, int y2, const engine::computed_value<float> &v );

plane create_random_plane(
    int x1, int y1, int x2, int y2, uint32_t seed, float minV, float maxV );
plane create_iotaX_plane( int x1, int y1, int x2, int y2 );
plane create_iotaY_plane( int x1, int y1, int x2, int y2 );

plane pad( const plane &a, int l, int t, int r, int b, float val );
plane pad_hold( const plane &a, int l, int t, int r, int b );

/// returns a new a, padded to match a potentially larger b
plane union_dim( const plane &a, const plane &b, float outside = 0.F );
plane union_dim_hold( const plane &a, const plane &b );

// fill the border with dirichlet border conditions (0)
plane dirichlet( const plane &p, int border = 1 );

inline plane filter_nan( const plane &p, float repl = 0.F )
{
    return plane( "p.filter_nan", p.dims(), p, repl );
}

inline plane abs( const plane &p ) { return plane( "p.abs", p.dims(), p ); }
inline plane abs( plane &&p )
{
    return plane( "p.abs", p.dims(), std::move( p ) );
}

inline plane copysign( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to copysign for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.copysign_pp", a.dims(), a, b );
}

inline plane square( const plane &p )
{
    return plane( "p.square", p.dims(), p );
}
inline plane square( plane &&p )
{
    return plane( "p.square", p.dims(), std::move( p ) );
}

inline plane fma( const plane &p, float b, float c )
{
    return plane( "p.fma_pnn", p.dims(), p, b, c );
}
//inline plane lerp( const plane &a, const plane &b, float amt )
//{
//	// a * ( 1 - amt ) + b * amt;
//	return a + ( b - a ) * amt;
//}
//inline plane lerp( const plane &a, const plane &b, plane &amt )
//{
//	// a * ( 1 - amt ) + b * amt;
//	return a + ( b - a ) * amt;
//}

inline plane sqrt( const plane &p ) { return plane( "p.sqrt", p.dims(), p ); }
inline plane sqrt( plane &&p )
{
    return plane( "p.sqrt", p.dims(), std::move( p ) );
}

inline plane magnitude( const plane &p )
{
    return plane( "p.abs", p.dims(), p );
}
inline plane magnitude( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to find magnitude for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.mag2", a.dims(), a, b );
}

inline plane magnitude( const plane &a, const plane &b, const plane &c )
{
    precondition(
        a.dims() == b.dims() && a.dims() == c.dims(),
        "unable to find magnitude for planes of different sizes a {0} vs b {1} vs c {2}",
        a.dims(),
        b.dims(),
        c.dims() );
    return plane( "p.mag3", a.dims(), a, b, c );
}

// exponential functions
inline plane exp( const plane &p ) { return plane( "p.exp", p.dims(), p ); }
inline plane log( const plane &p ) { return plane( "p.log", p.dims(), p ); }

inline plane expm1( const plane &p ) { return plane( "p.expm1", p.dims(), p ); }
inline plane log1p( const plane &p ) { return plane( "p.log1p", p.dims(), p ); }

inline plane exp2( const plane &p ) { return plane( "p.exp2", p.dims(), p ); }
inline plane log2( const plane &p ) { return plane( "p.log2", p.dims(), p ); }

inline plane pow( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to compute power for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.pow_pp", a.dims(), a, b );
}
inline plane pow( const plane &p, int i )
{
    return plane( "p.pow_pi", p.dims(), p, i );
}
inline plane pow( const plane &p, float v )
{
    return plane( "p.pow_pn", p.dims(), p, v );
}

// trig functions
inline plane atan2( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to compute atan2 for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.atan2", a.dims(), a, b );
}

// comparison operators

inline plane min( const plane &a, const engine::computed_value<float> &b )
{
    return plane( "p.min_pn", a.dims(), a, b );
}
inline plane min( const engine::computed_value<float> &a, const plane &b )
{
    return min( b, a );
}

inline plane min( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to compute min for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.min_pp", a.dims(), a, b );
}
inline plane max( const plane &a, const engine::computed_value<float> &b )
{
    return plane( "p.max_pn", a.dims(), a, b );
}
inline plane max( const engine::computed_value<float> &a, const plane &b )
{
    return max( b, a );
}
inline plane max( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to compute max for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.max_pp", a.dims(), a, b );
}

inline plane clamp(
    const plane &                        a,
    const engine::computed_value<float> &minv,
    const engine::computed_value<float> &maxv )
{
    return plane( "p.clamp_pnn", a.dims(), a, minv, maxv );
}
inline plane clamp( const plane &a, float minv, float maxv )
{
    return plane( "p.clamp_pnn", a.dims(), a, minv, maxv );
}

/// out = (a < b) ? c : d
inline plane if_less( const plane &a, float b, float c, float d )
{
    return plane( "p.if_less_fff", a.dims(), a, b, c, d );
}

/// out = (a < b) ? c : d
inline plane if_less( const plane &a, float b, float c, const plane &d )
{
    precondition(
        a.dims() == d.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs d {1}",
        a.dims(),
        d.dims() );
    return plane( "p.if_less_ffp", a.dims(), a, b, c, d );
}

/// out = (a < b) ? c : d
inline plane if_less( const plane &a, float b, const plane &c, const plane &d )
{
    precondition(
        a.dims() == c.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs c {1}",
        a.dims(),
        c.dims() );
    precondition(
        a.dims() == d.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs d {1}",
        a.dims(),
        d.dims() );
    return plane( "p.if_less_fpp", a.dims(), a, b, c, d );
}

/// out = (a < b) ? c : d
inline plane if_less( const plane &a, float b, const plane &c, float d )
{
    precondition(
        a.dims() == c.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs c {1}",
        a.dims(),
        c.dims() );
    return plane( "p.if_less_fpf", a.dims(), a, b, c, d );
}

/// out = (a < b) ? c : d
inline plane
if_less( const plane &a, const plane &b, const plane &c, const plane &d )
{
    precondition(
        a.dims() == b.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    precondition(
        a.dims() == c.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs c {1}",
        a.dims(),
        c.dims() );
    precondition(
        a.dims() == d.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs d {1}",
        a.dims(),
        d.dims() );
    return plane( "p.if_less_ppp", a.dims(), a, b, c, d );
}

/// out = (a > b) ? c : d
inline plane
if_greater( const plane &a, float b, const plane &c, const plane &d )
{
    precondition(
        a.dims() == c.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs c {1}",
        a.dims(),
        c.dims() );
    precondition(
        a.dims() == d.dims(),
        "unable to compute if_less for planes of different sizes a {0} vs d {1}",
        a.dims(),
        d.dims() );
    return plane( "p.if_greater_fpp", a.dims(), a, b, c, d );
}

/// out = ( a > t ) ? 1.F : 0.F;
inline plane threshold( const plane &a, float t )
{
    return plane( "p.threshold_f", a.dims(), a, t );
}
/// out = ( a > t ) ? 1.F : 0.F;
inline plane threshold( const plane &a, const plane &t )
{
    precondition(
        a.dims() == t.dims(),
        "unable to compute threshold for planes of different sizes a {0} vs t {1}",
        a.dims(),
        t.dims() );
    return plane( "p.threshold_p", a.dims(), a, t );
}

////////////////////////////////////////
// normal operators
////////////////////////////////////////

/// unary plane operators
inline plane operator-( const plane &p )
{
    return plane( "p.mul_pn", p.dims(), p, -1.F );
}
inline plane operator-( plane &&p )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), -1.F );
}

/// @defgroup Add two planes together, or add a value to a plane. We
/// enumerate all the variants explicitly so we can tag r-value
/// objects for re-use, non-cache, etc.
///
/// @{
inline plane operator+( const plane &a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to add planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.add_pp", a.dims(), a, b );
}

inline plane operator+( plane &&a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to add planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.add_pp", a.dims(), std::move( a ), b );
}

inline plane operator+( const plane &a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to add planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.add_pp", a.dims(), a, std::move( b ) );
}

inline plane operator+( plane &&a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to add planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.add_pp", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator+( const plane &p, float v )
{
    return plane( "p.add_pn", p.dims(), p, v );
}
inline plane operator+( const plane &p, const engine::computed_value<float> &v )
{
    return plane( "p.add_pn", p.dims(), p, v );
}
inline plane operator+( plane &&p, float v )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), v );
}
inline plane operator+( plane &&p, const engine::computed_value<float> &v )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), v );
}
inline plane operator+( float v, const plane &p )
{
    return plane( "p.add_pn", p.dims(), p, v );
}
inline plane operator+( const engine::computed_value<float> &v, const plane &p )
{
    return plane( "p.add_pn", p.dims(), p, v );
}
inline plane operator+( const engine::computed_value<float> &v, plane &&p )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), v );
}
inline plane operator+( float v, plane &&p )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), v );
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
inline plane &operator+=( plane &a, const engine::computed_value<float> &b )
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
    precondition(
        a.dims() == b.dims(),
        "unable to subtract planes of different sizes a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.sub_pp", a.dims(), a, b );
}

inline plane operator-( plane &&a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to subtract planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.sub_pp", a.dims(), std::move( a ), b );
}

inline plane operator-( const plane &a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to subtract planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.sub_pp", a.dims(), a, std::move( b ) );
}

inline plane operator-( plane &&a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to subtract planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.sub_pp", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator-( const plane &p, float v )
{
    return plane( "p.add_pn", p.dims(), p, -v );
}
inline plane operator-( const plane &p, const engine::computed_value<float> &v )
{
    return plane( "p.add_pn", p.dims(), p, -v );
}
inline plane operator-( plane &&p, float v )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), -v );
}
inline plane operator-( plane &&p, const engine::computed_value<float> &v )
{
    return plane( "p.add_pn", p.dims(), std::move( p ), -v );
}
inline plane operator-( float v, const plane &p )
{
    return plane( "p.fma_pnn", p.dims(), p, -1.F, v );
}
inline plane operator-( const engine::computed_value<float> &v, const plane &p )
{
    return plane( "p.fma_pnn", p.dims(), p, -1.F, v );
}
inline plane operator-( float v, plane &&p )
{
    return plane( "p.fma_pnn", p.dims(), std::move( p ), -1.F, v );
}
inline plane operator-( const engine::computed_value<float> &v, plane &&p )
{
    return plane( "p.fma_pnn", p.dims(), std::move( p ), -1.F, v );
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
inline plane &operator-=( plane &a, const engine::computed_value<float> &b )
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
    precondition(
        a.dims() == b.dims(),
        "unable to multiply planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.mul_pp", a.dims(), a, b );
}

inline plane operator*( plane &&a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to multiply planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.mul_pp", a.dims(), std::move( a ), b );
}

inline plane operator*( const plane &a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to multiply planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.mul_pp", a.dims(), a, std::move( b ) );
}

inline plane operator*( plane &&a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to multiply planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.mul_pp", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator*( const plane &p, float v )
{
    return plane( "p.mul_pn", p.dims(), p, v );
}
inline plane operator*( const plane &p, const engine::computed_value<float> &v )
{
    return plane( "p.mul_pn", p.dims(), p, v );
}
inline plane operator*( plane &&p, float v )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), v );
}
inline plane operator*( plane &&p, const engine::computed_value<float> &v )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), v );
}
inline plane operator*( float v, const plane &p )
{
    return plane( "p.mul_pn", p.dims(), p, v );
}
inline plane operator*( const engine::computed_value<float> &v, const plane &p )
{
    return plane( "p.mul_pn", p.dims(), p, v );
}
inline plane operator*( float v, plane &&p )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), v );
}
inline plane operator*( const engine::computed_value<float> &v, plane &&p )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), v );
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
inline plane &operator*=( plane &a, const engine::computed_value<float> &b )
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
    precondition(
        a.dims() == b.dims(),
        "unable to divide planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.div_pp", a.dims(), a, b );
}

inline plane operator/( plane &&a, const plane &b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to divide planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.div_pp", a.dims(), std::move( a ), b );
}

inline plane operator/( const plane &a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to divide planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.div_pp", a.dims(), a, std::move( b ) );
}

inline plane operator/( plane &&a, plane &&b )
{
    precondition(
        a.dims() == b.dims(),
        "unable to divide planes of different a {0} vs b {1}",
        a.dims(),
        b.dims() );
    return plane( "p.div_pp", a.dims(), std::move( a ), std::move( b ) );
}

inline plane operator/( const plane &p, float v )
{
    return plane( "p.mul_pn", p.dims(), p, 1.F / v );
}
inline plane operator/( const plane &p, const engine::computed_value<float> &v )
{
    return plane( "p.mul_pn", p.dims(), p, 1.F / v );
}
inline plane operator/( plane &&p, float v )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), 1.F / v );
}
inline plane operator/( plane &&p, const engine::computed_value<float> &v )
{
    return plane( "p.mul_pn", p.dims(), std::move( p ), 1.F / v );
}
inline plane operator/( float v, const plane &p )
{
    return plane( "p.div_np", p.dims(), v, p );
}
inline plane operator/( const engine::computed_value<float> &v, const plane &p )
{
    return plane( "p.div_np", p.dims(), v, p );
}
inline plane operator/( float v, plane &&p )
{
    return plane( "p.div_np", p.dims(), v, std::move( p ) );
}
inline plane operator/( const engine::computed_value<float> &v, plane &&p )
{
    return plane( "p.div_np", p.dims(), v, std::move( p ) );
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
inline plane &operator/=( plane &a, const engine::computed_value<float> &b )
{
    a = a / b;
    return a;
}
/// @}

} // namespace image
