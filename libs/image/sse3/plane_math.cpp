// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "plane_math.h"

#ifdef __SSE__
#    if defined( LINUX ) || defined( __linux__ )
#        include <x86intrin.h>
#    else
#        include <immintrin.h>
#        include <xmmintrin.h>
#    endif
#endif

////////////////////////////////////////

namespace image
{
namespace sse3
{
////////////////////////////////////////

void assign_value( image::scanline &dest, float v )
{
    __m128 vx = _mm_set1_ps( v );
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( vx, c );
}

////////////////////////////////////////

void add_planeplane(
    scanline &dest, const scanline &srcA, const scanline &srcB )
{
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_add_ps( srcA.load4( c ), srcB.load4( c ) ), c );
}

////////////////////////////////////////

void add_planenumber( scanline &dest, const scanline &srcA, float v )
{
    __m128 vx = _mm_set1_ps( v );
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_add_ps( srcA.load4( c ), vx ), c );
}

////////////////////////////////////////

void sub_planeplane(
    scanline &dest, const scanline &srcA, const scanline &srcB )
{
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_sub_ps( srcA.load4( c ), srcB.load4( c ) ), c );
}

////////////////////////////////////////

void mul_planeplane(
    scanline &dest, const scanline &srcA, const scanline &srcB )
{
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_mul_ps( srcA.load4( c ), srcB.load4( c ) ), c );
}

////////////////////////////////////////

void mul_planenumber( scanline &dest, const scanline &srcA, float v )
{
    __m128 vx = _mm_set1_ps( v );
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_mul_ps( srcA.load4( c ), vx ), c );
}

////////////////////////////////////////

void div_planeplane(
    scanline &dest, const scanline &srcA, const scanline &srcB )
{
    __m128 z = _mm_setzero_ps();
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
    {
        __m128 b     = srcB.load4( c );
        __m128 zMask = _mm_cmpeq_ps( b, z );
        __m128 out   = _mm_or_ps(
            _mm_andnot_ps( zMask, _mm_div_ps( srcA.load4( c ), b ) ),
            _mm_and_ps( zMask, b ) );
        dest.store4( out, c );
    }
}

////////////////////////////////////////

void div_numberplane( scanline &dest, float v, const scanline &src )
{
    __m128 z  = _mm_setzero_ps();
    __m128 vx = _mm_set1_ps( v );
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
    {
        __m128 b     = src.load4( c );
        __m128 zMask = _mm_cmpeq_ps( b, z );
        __m128 out   = _mm_or_ps(
            _mm_andnot_ps( zMask, _mm_div_ps( vx, b ) ),
            _mm_and_ps( zMask, b ) );
        dest.store4( out, c );
    }
}

////////////////////////////////////////

void muladd_planeplaneplane(
    scanline &      dest,
    const scanline &srcA,
    const scanline &srcB,
    const scanline &srcC )
{
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4(
            _mm_add_ps(
                _mm_mul_ps( srcA.load4( c ), srcB.load4( c ) ),
                srcC.load4( c ) ),
            c );
}

////////////////////////////////////////

void muladd_planenumbernumber(
    scanline &dest, const scanline &src, float a, float b )
{
    __m128 va = _mm_set1_ps( a );
    __m128 vb = _mm_set1_ps( b );
    for ( int c = 0, C = dest.chunks4(); c != C; ++c )
        dest.store4( _mm_add_ps( _mm_mul_ps( src.load4( c ), va ), vb ), c );
}

////////////////////////////////////////

} // namespace sse3
} // namespace image
