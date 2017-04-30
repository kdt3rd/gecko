//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "plane_math.h"
#include "scanline_process.h"
#include <base/cpu_features.h>
#include <iostream>
#include <cmath>
#include <random>

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

static void random_value( scanline &dest, int y, uint32_t seed, float minV, float maxV )
{
	std::mt19937 genY( seed );
	// does this just run the engine y*W times or does it fast jump?
	// if it's slow, we should have 2, one to generate seeds for
	// lines, one for each scanline
	if ( y > static_cast<int>( minV ) )
		genY.discard( static_cast<unsigned long long>( y - static_cast<int>( minV ) ) );
	std::mt19937 genX( genY() );
	std::uniform_real_distribution<float> urd( minV, std::nextafter( maxV, std::numeric_limits<float>::max() ) );

	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = urd( genX );
}

////////////////////////////////////////

static void iotaX_value( scanline &dest )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = static_cast<float>( x + dest.offset() );
}

////////////////////////////////////////

static void iotaY_value( scanline &dest, int y )
{
	float yV = static_cast<float>( y );
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = yV;
}

////////////////////////////////////////

static void
fill_pad( scanline &dest, int y, const plane &p, float val )
{
	int n = dest.width();
	if ( y < p.y1() || y > p.y2() )
	{
		for ( int x = 0; x < n; ++x )
			dest[x] = val;
		return;
	}

	int curx = dest.offset();
	auto i = dest.begin();
	auto e = dest.end();
	while ( curx < p.x1() )
	{
		*i++ = val;
		++curx;
	}
	const float *line = p.line( y );
	int inW = p.width();
	memcpy( i, line, inW * sizeof(float) );
	i += inW;

	while ( i < e )
		*i++ = val;
}

////////////////////////////////////////

static void
fill_pad_hold( scanline &dest, int y, const plane &p )
{
	int srcY = std::max( std::min( p.y2(), y ), p.y1() );

	int curx = dest.offset();
	const float *line = p.line( srcY );
	float leftV = line[0];
	auto i = dest.begin();
	auto e = dest.end();
	while ( curx < p.x1() )
	{
		*i++ = leftV;
		++curx;
	}
	int inW = p.width();
	memcpy( i, line, inW * sizeof(float) );
	i += inW;

	float rightV = line[inW - 1];
	while ( i < e )
		*i++ = rightV;
}

////////////////////////////////////////

static void
fill_dirichlet( scanline &dest, int y, const plane &p, int border )
{
	int n = dest.width();
	if ( y < ( p.y1() + border ) || y > ( p.y2() - border ) )
	{
		for ( int x = 0; x < n; ++x )
			dest[x] = 0.F;
		return;
	}
	const float *line = p.line( y );
	int endx = std::max( int(0), n - border );
	border = std::min( border, n );
	for ( int x = 0; x < border; ++x )
		dest[x] = 0.F;
	for ( int x = border; x < endx; ++x )
		dest[x] = line[x];
	for ( int x = endx; x < n; ++x )
		dest[x] = 0.F;
}

////////////////////////////////////////

static void plane_filter_nan( scanline &dest, const scanline &src, float repl )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float v = src[x];
		if ( std::isnan( v ) )
			v = repl;
		dest[x] = v;
	}
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

static void plane_abs( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fabsf( src[x] );
}

////////////////////////////////////////

static void plane_copysign( scanline &dest, const scanline &src, const scanline &v )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = copysignf( src[x], v[x] );
}

////////////////////////////////////////

static void plane_square( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float a = src[x];
		dest[x] = a * a;
	}
}

////////////////////////////////////////

static void plane_sqrt( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = sqrtf( src[x] );
}

////////////////////////////////////////

static void plane_mag2( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float a = srcA[x];
		float b = srcB[x];
		dest[x] = sqrtf( a*a + b*b );
	}
}

////////////////////////////////////////

static void plane_mag3( scanline &dest, const scanline &srcA, const scanline &srcB, const scanline &srcC )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
	{
		float a = srcA[x];
		float b = srcB[x];
		float c = srcC[x];
		dest[x] = sqrtf( a*a + b*b + c*c );
	}
}

////////////////////////////////////////

static void plane_exp( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = expf( src[x] );
}

////////////////////////////////////////

static void plane_log( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = logf( src[x] );
}

////////////////////////////////////////

static void plane_expm1( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = expm1f( src[x] );
}

////////////////////////////////////////

static void plane_log1p( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = log1pf( src[x] );
}

////////////////////////////////////////

static void plane_exp2( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = exp2f( src[x] );
}

////////////////////////////////////////

static void plane_log2( scanline &dest, const scanline &src )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = log2f( src[x] );
}

////////////////////////////////////////

static void plane_powp( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = powf( srcA[x], srcB[x] );
}

////////////////////////////////////////

static void plane_powi( scanline &dest, const scanline &srcA, int p )
{
	switch ( p )
	{
		case -1:
			for ( int x = 0, N = dest.width(); x != N; ++x )
				dest[x] = 1.F / srcA[x];
			break;
		case 0:
			for ( int x = 0, N = dest.width(); x != N; ++x )
				dest[x] = 1.F;
			break;
		case 1:
			if ( dest != srcA )
			{
				for ( int x = 0, N = dest.width(); x != N; ++x )
					dest[x] = srcA[x];
			}
			break;
		case 2:
			plane_square( dest, srcA );
			break;
		case 3:
			for ( int x = 0, N = dest.width(); x != N; ++x )
			{
				float a = srcA[x];
				dest[x] = a*a*a;
			}
			break;
		case 4:
			for ( int x = 0, N = dest.width(); x != N; ++x )
			{
				float a = srcA[x];
				a = a*a;
				dest[x] = a*a;
			}
			break;
		default:
			for ( int x = 0, N = dest.width(); x != N; ++x )
				dest[x] = powf( srcA[x], p );
			break;
	}
}

////////////////////////////////////////

static void plane_powf( scanline &dest, const scanline &srcA, float v )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = powf( srcA[x], v );
}

////////////////////////////////////////

static void plane_atan2( scanline &dest, const scanline &srcA, const scanline &srcB )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = atan2f( srcA[x], srcB[x] );
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////

static void plane_minpp( scanline &dest, const scanline &a, const scanline &b )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fminf( a[x], b[x] );
}

static void plane_minpn( scanline &dest, const scanline &a, float b )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fminf( a[x], b );
}

////////////////////////////////////////

static void plane_maxpp( scanline &dest, const scanline &a, const scanline &b )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fmaxf( a[x], b[x] );
}

static void plane_maxpn( scanline &dest, const scanline &a, float b )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fmaxf( a[x], b );
}

////////////////////////////////////////

static void plane_clamp_pnn( scanline &dest, const scanline &a, float minV, float maxV )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = fmaxf( fminf( maxV, a[x] ), minV );
}

////////////////////////////////////////

static void plane_ifless_fff( scanline &dest, const scanline &a, float b, float c, float d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] < b ? c : d;
}

////////////////////////////////////////

static void plane_ifless_ffp( scanline &dest, const scanline &a, float b, float c, const scanline &d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] < b ? c : d[x];
}

////////////////////////////////////////

static void plane_ifless_fpp( scanline &dest, const scanline &a, float b, const scanline &c, const scanline &d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] < b ? c[x] : d[x];
}

////////////////////////////////////////

static void plane_ifless_fpf( scanline &dest, const scanline &a, float b, const scanline &c, float d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] < b ? c[x] : d;
}

////////////////////////////////////////

static void plane_ifless_ppp( scanline &dest, const scanline &a, const scanline &b, const scanline &c, const scanline &d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] < b[x] ? c[x] : d[x];
}

////////////////////////////////////////

static void plane_ifgreater( scanline &dest, const scanline &a, float b, const scanline &c, const scanline &d )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] > b ? c[x] : d[x];
}

////////////////////////////////////////

static void plane_thresholdf( scanline &dest, const scanline &a, float t )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] > t ? 1.F : 0.F;
}
static void plane_thresholdp( scanline &dest, const scanline &a, const scanline &t )
{
	for ( int x = 0, N = dest.width(); x != N; ++x )
		dest[x] = a[x] > t[x] ? 1.F : 0.F;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void add_plane_math( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.assign", base::choose_runtime( assign_value, { { base::cpu::simd_feature::SSE3, sse3::assign_value } } ), scanline_plane_adapter<true, decltype(assign_value)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.random", base::choose_runtime( random_value ), n_scanline_plane_adapter<false, decltype(random_value)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.iota_x", base::choose_runtime( iotaX_value ), scanline_plane_adapter<true, decltype(iotaX_value)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.iota_y", base::choose_runtime( iotaY_value ), n_scanline_plane_adapter<true, decltype(iotaY_value)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.pad", base::choose_runtime( fill_pad ), n_scanline_plane_adapter<true, decltype(fill_pad)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.pad_hold", base::choose_runtime( fill_pad_hold ), n_scanline_plane_adapter<true, decltype(fill_pad_hold)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.filter_nan", base::choose_runtime( plane_filter_nan ), scanline_plane_adapter<true, decltype(plane_filter_nan)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.dirichlet", base::choose_runtime( fill_dirichlet ), n_scanline_plane_adapter<true, decltype(fill_dirichlet)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.add_pp", base::choose_runtime( add_planeplane, { { base::cpu::simd_feature::SSE3, sse3::add_planeplane } } ), scanline_plane_adapter<true, decltype(add_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.add_pn", base::choose_runtime( add_planenumber, { { base::cpu::simd_feature::SSE3, sse3::add_planenumber } } ), scanline_plane_adapter<true, decltype(add_planenumber)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.sub_pp", base::choose_runtime( sub_planeplane, { { base::cpu::simd_feature::SSE3, sse3::sub_planeplane } } ), scanline_plane_adapter<true, decltype(sub_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.mul_pp", base::choose_runtime( mul_planeplane, { { base::cpu::simd_feature::SSE3, sse3::mul_planeplane } } ), scanline_plane_adapter<true, decltype(mul_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.mul_pn", base::choose_runtime( mul_planenumber, { { base::cpu::simd_feature::SSE3, sse3::mul_planenumber } } ), scanline_plane_adapter<true, decltype(mul_planenumber)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.div_pp", base::choose_runtime( div_planeplane, { { base::cpu::simd_feature::SSE3, sse3::div_planeplane }, { base::cpu::simd_feature::SSE42, sse4::div_planeplane } } ), scanline_plane_adapter<true, decltype(div_planeplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.div_np", base::choose_runtime( div_numberplane, { { base::cpu::simd_feature::SSE3, sse3::div_numberplane }, { base::cpu::simd_feature::SSE42, sse4::div_numberplane } } ), scanline_plane_adapter<true, decltype(div_numberplane)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.fma_ppp", base::choose_runtime( muladd_planeplaneplane, { { base::cpu::simd_feature::SSE3, sse3::muladd_planeplaneplane } } ), scanline_plane_adapter<true, decltype(muladd_planeplaneplane)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.fma_pnn", base::choose_runtime( muladd_planenumbernumber, { { base::cpu::simd_feature::SSE3, sse3::muladd_planenumbernumber } } ), scanline_plane_adapter<true, decltype(muladd_planenumbernumber)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.abs", base::choose_runtime( plane_abs ), scanline_plane_adapter<true, decltype(plane_abs)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.copysign_pp", base::choose_runtime( plane_copysign ), scanline_plane_adapter<true, decltype(plane_copysign)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.square", base::choose_runtime( plane_square ), scanline_plane_adapter<true, decltype(plane_square)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sqrt", base::choose_runtime( plane_sqrt ), scanline_plane_adapter<true, decltype(plane_sqrt)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.mag2", base::choose_runtime( plane_mag2 ), scanline_plane_adapter<true, decltype(plane_mag2)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.mag3", base::choose_runtime( plane_mag3 ), scanline_plane_adapter<true, decltype(plane_mag3)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.exp", base::choose_runtime( plane_exp ), scanline_plane_adapter<true, decltype(plane_exp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.log", base::choose_runtime( plane_log ), scanline_plane_adapter<true, decltype(plane_log)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.expm1", base::choose_runtime( plane_expm1 ), scanline_plane_adapter<true, decltype(plane_expm1)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.log1p", base::choose_runtime( plane_log1p ), scanline_plane_adapter<true, decltype(plane_log1p)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.exp2", base::choose_runtime( plane_exp2 ), scanline_plane_adapter<true, decltype(plane_exp2)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.log2", base::choose_runtime( plane_log2 ), scanline_plane_adapter<true, decltype(plane_log2)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.pow_pp", base::choose_runtime( plane_powp ), scanline_plane_adapter<true, decltype(plane_powp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.pow_pi", base::choose_runtime( plane_powi ), scanline_plane_adapter<true, decltype(plane_powi)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.pow_pn", base::choose_runtime( plane_powf ), scanline_plane_adapter<true, decltype(plane_powf)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.atan2", base::choose_runtime( plane_atan2 ), scanline_plane_adapter<true, decltype(plane_atan2)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.min_pn", base::choose_runtime( plane_minpn ), scanline_plane_adapter<true, decltype(plane_minpn)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.min_pp", base::choose_runtime( plane_minpp ), scanline_plane_adapter<true, decltype(plane_minpp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.max_pn", base::choose_runtime( plane_maxpn ), scanline_plane_adapter<true, decltype(plane_maxpn)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.max_pp", base::choose_runtime( plane_maxpp ), scanline_plane_adapter<true, decltype(plane_maxpp)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.clamp_pnn", base::choose_runtime( plane_clamp_pnn ), scanline_plane_adapter<true, decltype(plane_clamp_pnn)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.if_less_fff", base::choose_runtime( plane_ifless_fff ), scanline_plane_adapter<true, decltype(plane_ifless_fff)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.if_less_ffp", base::choose_runtime( plane_ifless_ffp ), scanline_plane_adapter<true, decltype(plane_ifless_ffp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.if_less_fpp", base::choose_runtime( plane_ifless_fpp ), scanline_plane_adapter<true, decltype(plane_ifless_fpp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.if_less_fpf", base::choose_runtime( plane_ifless_fpf ), scanline_plane_adapter<true, decltype(plane_ifless_fpf)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.if_less_ppp", base::choose_runtime( plane_ifless_ppp ), scanline_plane_adapter<true, decltype(plane_ifless_ppp)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.if_greater_fpp", base::choose_runtime( plane_ifgreater ), scanline_plane_adapter<true, decltype(plane_ifgreater)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.threshold_f", base::choose_runtime( plane_thresholdf ), scanline_plane_adapter<true, decltype(plane_thresholdf)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.threshold_p", base::choose_runtime( plane_thresholdp ), scanline_plane_adapter<true, decltype(plane_thresholdp)>(), dispatch_scan_processing, op::one_to_one ) );
}

////////////////////////////////////////

} // image



