// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "plane_math.h"

#ifdef __SSE__
# if defined(LINUX) || defined(__linux__)
#  include <x86intrin.h>
# else
#  include <xmmintrin.h>
#  include <immintrin.h>
# endif
#endif

////////////////////////////////////////

namespace image
{
namespace sse4
{

////////////////////////////////////////

void div_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	__m128 z = _mm_setzero_ps();
	for ( int c = 0, C = dest.chunks4(); c != C; ++c )
	{
		__m128 b = srcB.load4( c );
		__m128 zMask = _mm_cmpeq_ps( b, z );
		dest.store4( _mm_blendv_ps( _mm_div_ps( srcA.load4( c ), b ), b, zMask ), c );
	}
}

////////////////////////////////////////

void div_numberplane( scanline &dest, float v, const scanline &src )
{
	__m128 z = _mm_setzero_ps();
	__m128 vx = _mm_set1_ps( v );
	for ( int c = 0, C = dest.chunks4(); c != C; ++c )
	{
		__m128 b = src.load4( c );
		__m128 zMask = _mm_cmpeq_ps( b, z );
		dest.store4( _mm_blendv_ps( _mm_div_ps( vx, b ), b, zMask ), c );
	}
}

////////////////////////////////////////

} // sse4
} // image



