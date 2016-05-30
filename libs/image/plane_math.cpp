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
#include "scanline_process.h"
#include <base/cpu_features.h>
#include <iostream>
// TODO: add ifdefs when compiling for alternate platforms (i.e. ARM)
#include "sse3/plane_math.h"
#include "sse4/plane_math.h"

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

static void assign_value( scanline &dest, float v )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = v;
}

////////////////////////////////////////

static void add_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] + srcB[x];
}

////////////////////////////////////////

static void add_planenumber( scanline &dest, const scanline &srcA, float v )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] - v;
}

////////////////////////////////////////

static void sub_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] - srcB[x];
}

////////////////////////////////////////

static void mul_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] * srcB[x];
}

////////////////////////////////////////

static void mul_planenumber( scanline &dest, const scanline &srcA, float v )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] * v;
}

////////////////////////////////////////

static void div_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float b = srcB[x];
		if ( b == 0.F )
			dest[x] = b;
		else
			dest[x] = srcA[x] / b;
	}
}

////////////////////////////////////////

static void div_numberplane( scanline &dest, float v, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float a = src[x];
		if ( a == 0.F )
			dest[x] = a;
		else
			dest[x] = v / a;
	}
}

////////////////////////////////////////

static void muladd_planeplaneplane( scanline &dest, const scanline &srcA, const scanline &srcB, const scanline &srcC )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = srcA[x] * srcB[x] + srcC[x];
}

////////////////////////////////////////

static void muladd_planenumbernumber( scanline &dest, const scanline &src, float a, float b )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = src[x] * a + b;
}

////////////////////////////////////////

void add_plane_math( engine::registry &r )
{
	using namespace engine;
//	r.add( op( "add_planeplane", base::choose_runtime( add_planeplane, { { cpu::simd_feature::AVX, avx::add_planeplane } } ), scanline_plane_operator( add_planeplane ), op::one_to_one ) );

	r.add( op( "assign_plane", base::choose_runtime( assign_value, { { base::cpu::simd_feature::SSE3, sse3::assign_value } } ), scanline_plane_adapter<decltype(assign_value)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "add_planeplane", base::choose_runtime( add_planeplane, { { base::cpu::simd_feature::SSE3, sse3::add_planeplane } } ), scanline_plane_adapter<decltype(add_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "add_planenumber", base::choose_runtime( add_planenumber, { { base::cpu::simd_feature::SSE3, sse3::add_planenumber } } ), scanline_plane_adapter<decltype(add_planenumber)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "sub_planeplane", base::choose_runtime( sub_planeplane, { { base::cpu::simd_feature::SSE3, sse3::sub_planeplane } } ), scanline_plane_adapter<decltype(sub_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "mul_planeplane", base::choose_runtime( mul_planeplane, { { base::cpu::simd_feature::SSE3, sse3::mul_planeplane } } ), scanline_plane_adapter<decltype(mul_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "mul_planenumber", base::choose_runtime( mul_planenumber, { { base::cpu::simd_feature::SSE3, sse3::mul_planenumber } } ), scanline_plane_adapter<decltype(mul_planenumber)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "div_planeplane", base::choose_runtime( div_planeplane, { { base::cpu::simd_feature::SSE3, sse3::div_planeplane }, { base::cpu::simd_feature::SSE42, sse4::div_planeplane } } ), scanline_plane_adapter<decltype(div_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "div_numberplane", base::choose_runtime( div_numberplane, { { base::cpu::simd_feature::SSE3, sse3::div_numberplane }, { base::cpu::simd_feature::SSE42, sse4::div_numberplane } } ), scanline_plane_adapter<decltype(div_numberplane)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "muladd_planeplaneplane", base::choose_runtime( muladd_planeplaneplane, { { base::cpu::simd_feature::SSE3, sse3::muladd_planeplaneplane } } ), scanline_plane_adapter<decltype(muladd_planeplaneplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "muladd_planenumbernumber", base::choose_runtime( muladd_planenumbernumber, { { base::cpu::simd_feature::SSE3, sse3::muladd_planenumbernumber } } ), scanline_plane_adapter<decltype(muladd_planenumbernumber)>(), dispatch_scan_processing, op::one_to_one ) );
}

////////////////////////////////////////

} // image



