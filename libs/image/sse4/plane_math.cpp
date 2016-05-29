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



