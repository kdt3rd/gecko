//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "optical_flow.h"
#include "plane_ops.h"
#include "threading.h"
#include "debug_util.h"
#include "media_io.h"
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <vector>

#ifdef __SSE__
# if defined(LINUX) || defined(__linux__)
#  include <x86intrin.h>
# else
#  include <xmmintrin.h>
#  include <immintrin.h>
# endif
#endif

////////////////////////////////////////

namespace
{
using namespace image;

//static const float kBigNum = 12345.F;

inline float mymaxf( float a, float b )
{
#if defined(__GNUC__)
	return __builtin_fmaxf( a, b );
#else
	return fmaxf( a, b );
#endif
}
inline float myminf( float a, float b )
{
#if defined(__GNUC__)
	return __builtin_fminf( a, b );
#else
	return fminf( a, b );
#endif
}

////////////////////////////////////////

inline plane
doGradX( const plane &a, const plane &alpha, float lambda = 10.F )
{
//	if ( alpha.valid() )
//		return instant_gradient_horiz( a, alpha );
//	return instant_gradient_horiz( a );
//	if ( alpha.valid() )
//		return central_gradient_horiz( a, alpha );
//	return central_gradient_horiz( a );
//	if ( alpha.valid() )
//		return noise_gradient_horiz5( a, alpha );
//	return noise_gradient_horiz5( a );

	plane ret;
	if ( alpha.valid() )
	{
		if ( a.width() <= 128 )
			ret = instant_gradient_horiz( a, alpha );
		else if ( a.width() <= 512 )
			ret = central_gradient_horiz( a, alpha );
		else
			ret = noise_gradient_horiz5( a, alpha );
	}
	else
	{
		if ( a.width() <= 128 )
			ret =  instant_gradient_horiz( a );
		else if ( a.width() <= 512 )
			ret = central_gradient_horiz( a );
		else
			ret = noise_gradient_horiz5( a );
	}

//	ret = ret * ( exp( square( ret ) * (- lambda * 0.5F ) ) + 0.01F );
	return ret;
}

////////////////////////////////////////

inline plane
doGradY( const plane &a, const plane &alpha, float lambda = 10.F )
{
//	if ( alpha.valid() )
//		return instant_gradient_vert( a, alpha );
//	return instant_gradient_vert( a );
//	if ( alpha.valid() )
//		return central_gradient_vert( a, alpha );
//	return central_gradient_vert( a );
//	if ( alpha.valid() )
//		return noise_gradient_vert5( a, alpha );
//	return noise_gradient_vert5( a );
	plane ret;
	if ( alpha.valid() )
	{
		// use width so it's the same as doGradX
		if ( a.width() <= 128 )
			ret = instant_gradient_vert( a, alpha );
		else if ( a.width() <= 512 )
			ret = central_gradient_vert( a, alpha );
		else
			ret = noise_gradient_vert5( a, alpha );
	}
	else
	{
		if ( a.width() <= 128 )
			ret = instant_gradient_vert( a );
		else if ( a.width() <= 512 )
			ret = central_gradient_vert( a );
		else
			ret = noise_gradient_vert5( a );
	}
//	ret = ret * ( exp( square( ret ) * (- lambda * 0.5F ) ) + 0.01F );
	return ret;
}

////////////////////////////////////////

static void
doKillAlpha( size_t, int s, int e, plane_buffer &u, plane_buffer &v, const const_plane_buffer &alpha )
{
	int w = u.width();
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		const float *alphaLine = alpha.line( y );
		for ( int x = 0; x < w; ++x )
		{
			if ( alphaLine[x] <= 0.F )
			{
				uLine[x] = 0.F;
				vLine[x] = 0.F;
			}
		}
	}
}

static void
killAlpha( plane &u, plane &v, const plane &alpha )
{
	if ( alpha.valid() )
	{
		plane_buffer uB = u;
		plane_buffer vB = v;
		const_plane_buffer alpB = alpha;
		threading::get().dispatch( std::bind( doKillAlpha, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( uB ), std::ref( vB ), std::cref( alpB ) ), alpha );
	}
}

////////////////////////////////////////

static void
doZilchAlpha( size_t, int s, int e, plane_buffer &u, plane_buffer &v, const const_plane_buffer &alpha )
{
	int w = u.width();
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		const float *alphaLine = alpha.line( y );
		for ( int x = 0; x < w; ++x )
		{
			if ( alphaLine[x] <= 0.F )
			{
				uLine[x] = static_cast<float>( -( x + 1 ) );
				vLine[x] = static_cast<float>( u.y1() - y - 1 );
			}
		}
	}
}

static void
zilchAlpha( plane &u, plane &v, const plane &alpha )
{
	if ( alpha.valid() )
	{
		plane_buffer uB = u;
		plane_buffer vB = v;
		const_plane_buffer alpB = alpha;
		threading::get().dispatch( std::bind( doZilchAlpha, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( uB ), std::ref( vB ), std::cref( alpB ) ), alpha );
	}
}


////////////////////////////////////////

static void
ahtvl1_edgeWeight_thread( size_t, int s, int e, plane_buffer &ew, const const_plane_buffer &eb, float ePower, float eWeight, int border )
{
	int w = eb.width();
	int exm1 = w - border - 1;

	for ( int y = s; y < e; ++y )
	{
		float *ewLine = ew.line( y );
		if ( y < border || y > ( eb.y2() - border ) )
		{
			for ( int x = 0; x < w; ++x )
				ewLine[x] = 1.F;
			continue;
		}
		const float *ebLine = eb.line( y );
		const float *ebm1Line = eb.line( y - 1 );
		const float *ebp1Line = eb.line( y + 1 );

		for ( int x = 0; x < border; ++x )
			ewLine[x] = 1.F;

		for ( int x = border; x <= exm1; ++x )
		{
			float emx = ebLine[x - 1];
			float epx = ebLine[x + 1];
			float emy = ebm1Line[x];
			float epy = ebp1Line[x];
			float gxv = ( epx - emx ) * 0.5F;
			float gyv = ( epy - emy ) * 0.5F;
			float norm = sqrtf( gxv * gxv + gyv * gyv );
			// eWeight negated in call, so we can just scale by it here
			ewLine[x] = expf( eWeight * powf( norm, ePower ) );
		}

		for ( int x = exm1 + 1; x < w; ++x )
			ewLine[x] = 1.F;
	}
}

////////////////////////////////////////

static void
ahtvl1_edgeWeight_alpha_thread( size_t, int s, int e, plane_buffer &ew, const const_plane_buffer &alpha, const const_plane_buffer &eb, float ePower, float eWeight, int border )
{
	int w = eb.width();
	int exm1 = w - border - 1;

	for ( int y = s; y < e; ++y )
	{
		const float *aLine = alpha.line( y );
		float *ewLine = ew.line( y );
		if ( y < ( ew.y1() + border ) || y > ( eb.y2() - border ) )
		{
			for ( int x = 0; x < w; ++x )
				ewLine[x] = aLine[x];
			continue;
		}
		const float *ebLine = eb.line( y );
		const float *ebm1Line = eb.line( y - 1 );
		const float *ebp1Line = eb.line( y + 1 );

		for ( int x = 0; x < border; ++x )
			ewLine[x] = aLine[x] > 0.F ? 1.F : 0.F;

		for ( int x = border; x <= exm1; ++x )
		{
			if ( aLine[x] <= 0.F )
			{
				ewLine[x] = 0.F;
				continue;
			}

			float emx = ebLine[x - 1];
			float epx = ebLine[x + 1];
			float emy = ebm1Line[x];
			float epy = ebp1Line[x];
			float gxv = ( epx - emx ) * 0.5F;
			float gyv = ( epy - emy ) * 0.5F;
			float norm = sqrtf( gxv * gxv + gyv * gyv );
			// eWeight negated in call, so we can just scale by it here
			ewLine[x] = expf( eWeight * powf( norm, ePower ) );
		}

		for ( int x = exm1 + 1; x < w; ++x )
			ewLine[x] = aLine[x] > 0.F ? 1.F : 0.F;
	}
}

static plane
ahtvl1_edgeWeight( const plane &curA, const plane &alpha, const std::vector<float> &edgeKern, float edgePower, float edgeAlpha, int edgeBorder )
{
	plane ret( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
	plane ba = separable_convolve( curA, edgeKern );
	plane_buffer eb = ret;
	const_plane_buffer bab = ba;

	if ( alpha.valid() )
	{
		const_plane_buffer alpb = alpha;
		
		threading::get().dispatch( std::bind( ahtvl1_edgeWeight_alpha_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( eb ), std::cref( alpb ), std::cref( bab ), edgePower, - edgeAlpha, edgeBorder ), curA );
	}
	else
	{
		threading::get().dispatch( std::bind( ahtvl1_edgeWeight_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( eb ), std::cref( bab ), edgePower, - edgeAlpha, edgeBorder ), curA );
	}


	return ret;
}

////////////////////////////////////////

static void
peakThread( size_t, int s, int e, plane_buffer &u, const const_plane_buffer &origu, int r )
{
	int w = u.width();
	int ew = w - r;
	int numSamps = 2 * r + 1;
	numSamps *= numSamps;
	float norm = 1.F / static_cast<float>( numSamps );
	int halfIdx = numSamps / 2;
	for ( int hy = s; hy < e; ++hy )
	{
		float *uLine = u.line( hy );
		const float *origLine = origu.line( hy );
		if ( hy < ( u.y1() + r ) || hy > ( u.y2() - r ) )
		{
			for ( int hx = 0; hx < w; ++hx )
				uLine[hx] = origLine[hx];
			continue;
		}
		for ( int hx = 0; hx < r; ++hx )
			uLine[hx] = origLine[hx];
		for ( int hx = r; hx < ew; ++hx )
		{
			int maxIdx = 0, curIdx = 0;
			float maxV = 0.F;
			float prevMaxV = 0.F;
			float uV = origLine[hx];
			float sumMU = 0.F;
			for ( int y = -r; y <= r; ++y )
			{
				int sy = hy + y;
				const float *filtLine = origu.line( sy );
				for ( int x = -r; x <= r; ++x )
				{
					float v = filtLine[hx + x];
					float av = fabsf( v );
					if ( av > maxV )
					{
						prevMaxV = maxV;
						maxV = av;
						maxIdx = curIdx;
					}
					else if ( av > prevMaxV )
						prevMaxV = av;

					sumMU += v;
					++curIdx;
				}
			}

			if ( maxIdx == halfIdx )
				uV = ( prevMaxV + sumMU * norm ) * 0.5F;

			uLine[hx] = uV;
		}
		for ( int hx = ew; hx < w; ++hx )
			uLine[hx] = origLine[hx];
	}
}

static void
removePeaks( plane &u, plane &v, const plane &origU, const plane &origV, int diam )
{
	{
		const_plane_buffer mub = origU;
		plane_buffer ub = u;
		threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::cref( mub ), static_cast<int>(diam/2) ), u );
	}

	const_plane_buffer mvb = origV;
	plane_buffer vb = v;
	threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( vb ), std::cref( mvb ), static_cast<int>(diam/2) ), v );
}

static void
removePeaks( plane &u, plane &v, int diam )
{
	{
		plane u0 = u.copy();
		const_plane_buffer mub = u0;
		plane_buffer ub = u;
		threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::cref( mub ), static_cast<int>(diam/2) ), u );
	}

	plane v0 = v.copy();
	const_plane_buffer mvb = v0;
	plane_buffer vb = v;
	threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( vb ), std::cref( mvb ), static_cast<int>(diam/2) ), v );
}

////////////////////////////////////////

static void
ahtvl1_T_thread( size_t, int s, int e, plane_buffer &t, const const_plane_buffer &b, const const_plane_buffer &a, plane_buffer &u0, plane_buffer &v0, const const_plane_buffer &u, const const_plane_buffer &v )
{
	int w = t.width();
	float offX = static_cast<float>( b.x1() );
	for ( int y = s; y < e; ++y )
	{
		float * __restrict__ tLine = t.line( y );
		float * __restrict__ u0Line = u0.line( y );
		float * __restrict__ v0Line = v0.line( y );
		const float * __restrict__ uLine = u.line( y );
		const float * __restrict__ vLine = v.line( y );
		const float * __restrict__ aLine = a.line( y );

		// because of the gather, SSE is slower...
		float curY = static_cast<float>( y );
		float curX = offX;
		for ( int x = 0; x < w; ++x, curX += 1.F )
		{
			float warpX = uLine[x];
			float warpY = vLine[x];
			u0Line[x] = warpX;
			v0Line[x] = warpY;

			warpX += curX;
			warpY += curY;

			float out = bilinear_zero( b, warpX, warpY ) - aLine[x];

			tLine[x] = out;
		}
	}
}

static void
ahtvl1_T_alpha_thread( size_t, int s, int e, plane_buffer &t, const const_plane_buffer &b, const const_plane_buffer &a, const const_plane_buffer &alpha, plane_buffer &u0, plane_buffer &v0, const const_plane_buffer &u, const const_plane_buffer &v )
{
	int w = t.width();
	float offX = static_cast<float>( b.x1() );
	for ( int y = s; y < e; ++y )
	{
		float * __restrict__ tLine = t.line( y );
		float * __restrict__ u0Line = u0.line( y );
		float * __restrict__ v0Line = v0.line( y );
		const float * __restrict__ uLine = u.line( y );
		const float * __restrict__ vLine = v.line( y );
		const float * __restrict__ aLine = a.line( y );
		const float * __restrict__ alpLine = alpha.line( y );

		float curY = static_cast<float>( y );
		float curX = offX;
		for ( int x = 0; x < w; ++x, curX += 1.F )
		{
			float out = 0.F;
			float warpX = uLine[x];
			float warpY = vLine[x];
			u0Line[x] = warpX;
			v0Line[x] = warpY;

			if ( alpLine[x] <= 0.F )
			{
				// force things to move, hopefully toward the nearest
				// non-alpha
				if ( fabsf( warpX ) > 0.F || fabsf( warpX ) > 0.F )
					out = 1.F;
			}
			else
			{
				warpX += curX;
				warpY += curY;

				out = bilinear_zero( b, warpX, warpY ) - aLine[x];
			}

			tLine[x] = out;
		}
	}
}

static void
ahtvl1_T( plane &t, const plane &curB, const plane &curA, const plane &curAlpha, plane &u0, plane &v0, const plane &u, const plane &v )
{
	plane_buffer tb = t;
	const_plane_buffer bb = curB;
	const_plane_buffer ab = curA;
	const_plane_buffer ub = u;
	const_plane_buffer vb = v;
	plane_buffer u0b = u0;
	plane_buffer v0b = v0;

	if ( curAlpha.valid() )
	{
		const_plane_buffer alpb = curAlpha;

		threading::get().dispatch( std::bind( ahtvl1_T_alpha_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( tb ), std::cref( bb ), std::cref( ab ), std::cref( alpb ), std::ref( u0b ), std::ref( v0b ), std::cref( ub ), std::cref( vb ) ), curB.y1(), curB.height() );
	}
	else
	{
		threading::get().dispatch( std::bind( ahtvl1_T_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( tb ), std::cref( bb ), std::cref( ab ), std::ref( u0b ), std::ref( v0b ), std::cref( ub ), std::cref( vb ) ), curB.y1(), curB.height() );
	}
}

////////////////////////////////////////

static void
ahtvl1_gradAve_thread( size_t, int s, int e, plane_buffer &gx, const const_plane_buffer &bx, const const_plane_buffer &ax, const const_plane_buffer &u, const const_plane_buffer &v )
{
	int w = gx.width();
	float offX = static_cast<float>( gx.x1() );

	for ( int y = s; y < e; ++y )
	{
		float * __restrict__ gxLine = gx.line( y );
		const float * __restrict__ uLine = u.line( y );
		const float * __restrict__ vLine = v.line( y );
		const float * __restrict__ aLine = ax.line( y );

		// gather operation, can't really use SSE here, but we can try
		// because of the gather (bilinear), SSE is slower...
		float curY = static_cast<float>( y );
		float curX = offX;
		for ( int x = 0; x < w; ++x, curX += 1.F )
		{
			float warpX = uLine[x];
			float warpY = vLine[x];
			warpX += curX;
			warpY += curY;

			gxLine[x] = ( aLine[x] + bilinear_zero( bx, warpX, warpY ) ) * 0.5F;
		}
	}
}

static void
ahtvl1_gradAve( plane &gxAve, const plane &bx, const plane &ax, const plane &u, const plane &v )
{
	plane_buffer gxb = gxAve;
	const_plane_buffer bb = bx;
	const_plane_buffer ab = ax;
	const_plane_buffer ub = u;
	const_plane_buffer vb = v;

	threading::get().dispatch( std::bind( ahtvl1_gradAve_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( gxb ), std::cref( bb ), std::cref( ab ), std::cref( ub ), std::cref( vb ) ), bx.y1(), bx.height() );
}

////////////////////////////////////////

static void
ahtvl1_updateU_thread( size_t, int s, int e,
					   plane_buffer &u, plane_buffer &v,
					   const const_plane_buffer &puu,
					   const const_plane_buffer &puv,
					   const const_plane_buffer &pvu,
					   const const_plane_buffer &pvv,
					   const const_plane_buffer &u0,
					   const const_plane_buffer &v0,
					   const const_plane_buffer &t,
					   const const_plane_buffer &gx,
					   const const_plane_buffer &gy,
					   float lamTheta, float theta )
{
	int w = u.width();

	for ( int y = s; y < e; ++y )
	{
		float * __restrict__ uLine = u.line( y );
		float * __restrict__ vLine = v.line( y );
		const float * __restrict__ tLine = t.line( y );
		const float * __restrict__ gxLine = gx.line( y );
		const float * __restrict__ gyLine = gy.line( y );
		const float * __restrict__ u0Line = u0.line( y );
		const float * __restrict__ v0Line = v0.line( y );

		const float * __restrict__ puuLine = puu.line( y );
		const float * __restrict__ puvLine = puv.line( y );
		const float * __restrict__ pvuLine = pvu.line( y );
		const float * __restrict__ pvvLine = pvv.line( y );
		if ( y == u.y1() )
		{
			for ( int x = 0; x < w; ++x )
			{
				float rho = tLine[x];
				float gXAve = gxLine[x];
				float gYAve = gyLine[x];
				float initU = uLine[x];
				float initV = vLine[x];

				rho += ( initU - u0Line[x] ) * gXAve;
				rho += ( initV - v0Line[x] ) * gYAve;
				float magSq = gXAve * gXAve + gYAve * gYAve;

				float u_ = initU, v_ = initV;

				if ( rho < ( magSq * ( - lamTheta ) ) )
				{
					u_ += gXAve * lamTheta;
					v_ += gYAve * lamTheta;
				}
				else if ( rho > ( magSq * lamTheta ) )
				{
					u_ -= gXAve * lamTheta;
					v_ -= gYAve * lamTheta;
				}
				else if ( magSq > 0.F )
				{
					u_ -= rho * gXAve / magSq;
					v_ -= rho * gYAve / magSq;
				}

				float p0 = 0.F;
				if ( x > 0 )
					p0 = puuLine[x] - puuLine[x - 1];

				uLine[x] = u_ + p0 * theta;

				// else will still be 0 from above
				if ( x > 0 )
					p0 = pvuLine[x] - pvuLine[x - 1];

				vLine[x] = v_ + p0 * theta;
			}
			continue;
		}

		const float * __restrict__ puvm1L = puv.line( y - 1 );
		const float * __restrict__ pvvm1L = pvv.line( y - 1 );

#if defined(__SSSE3__)
		int sseChunks = (w+3)/4;
		sseChunks = sseChunks * 4;
		const __m128 lamThetaV = _mm_set1_ps( lamTheta );
		const __m128 thetaV = _mm_set1_ps( theta );
		const __m128 negZeroV = _mm_set1_ps( -0.F );

		__m128i puuPrev = _mm_set1_epi32( *(reinterpret_cast<const int *>(puuLine)) );
		__m128i pvuPrev = _mm_set1_epi32( *(reinterpret_cast<const int *>(pvuLine)) );
		for ( int x = 0; x < sseChunks; x += 4 )
		{
			__m128 rho = _mm_load_ps( tLine + x );
			__m128 u_ = _mm_load_ps( uLine + x );
			__m128 u0 = _mm_load_ps( u0Line + x );
			__m128 gXAve = _mm_load_ps( gxLine + x );
			__m128 v_ = _mm_load_ps( vLine + x );
			__m128 v0 = _mm_load_ps( v0Line + x );
			__m128 gYAve = _mm_load_ps( gyLine + x );

			__m128i tmppuu = _mm_load_si128( reinterpret_cast<const __m128i *>( puuLine + x ) );
			__m128 puv_v = _mm_load_ps( puvLine + x );
			__m128 puvm1_v = _mm_load_ps( puvm1L + x );

			rho = _mm_add_ps( rho, _mm_mul_ps( gXAve, _mm_sub_ps( u_, u0 ) ) );
			rho = _mm_add_ps( rho, _mm_mul_ps( gYAve, _mm_sub_ps( v_, v0 ) ) );

			__m128 magSq = _mm_add_ps(
				_mm_mul_ps( gXAve, gXAve ),
				_mm_mul_ps( gYAve, gYAve ) );

			__m128 testVal = _mm_mul_ps( magSq, lamThetaV );
			__m128 rhoLT = _mm_cmplt_ps( rho, _mm_xor_ps( testVal, negZeroV ) );
			__m128 rhoGT = _mm_cmpgt_ps( rho, testVal );
			// avoid divide by zero when magSq == 0 AND rho == 0
			__m128 rhoMagSq = _mm_cmpgt_ps( magSq, _mm_setzero_ps() );
			rhoMagSq = _mm_andnot_ps( _mm_or_ps( rhoLT, rhoGT ), rhoMagSq );

			__m128 gxScale = _mm_mul_ps( gXAve, lamThetaV );
			__m128 gxScaleRho = _mm_min_ps( thetaV, _mm_mul_ps( rho, _mm_div_ps( gXAve, magSq ) ) );

			u_ = _mm_add_ps( u_, _mm_and_ps( rhoLT, gxScale ) );
			u_ = _mm_sub_ps( u_, _mm_and_ps( rhoGT, gxScale ) );
			u_ = _mm_sub_ps( u_, _mm_and_ps( rhoMagSq, gxScaleRho ) );

			__m128 puu_v = _mm_castsi128_ps( tmppuu );
			__m128 puu_v_m1 = _mm_castsi128_ps( _mm_alignr_epi8( tmppuu, puuPrev, 12 ) );
			puuPrev = tmppuu;

			__m128 p0 = _mm_sub_ps( puu_v, puu_v_m1 );
			__m128 p1 = _mm_sub_ps( puv_v, puvm1_v );

			__m128i tmppvu = _mm_load_si128( reinterpret_cast<const __m128i *>( pvuLine + x ) );
			__m128 pvv_v = _mm_load_ps( pvvLine + x );
			__m128 pvvm1_v = _mm_load_ps( pvvm1L + x );

			u_ = _mm_add_ps( u_, _mm_mul_ps( thetaV, _mm_add_ps( p0, p1 ) ) );
			_mm_store_ps( uLine + x, u_ );

			__m128 gyScale = _mm_mul_ps( gYAve, lamThetaV );
			__m128 gyScaleRho = _mm_min_ps( thetaV, _mm_mul_ps( rho, _mm_div_ps( gYAve, magSq ) ) );

			v_ = _mm_add_ps( v_, _mm_and_ps( rhoLT, gyScale ) );
			v_ = _mm_sub_ps( v_, _mm_and_ps( rhoGT, gyScale ) );
			v_ = _mm_sub_ps( v_, _mm_and_ps( rhoMagSq, gyScaleRho ) );

			__m128 pvu_v = _mm_castsi128_ps( tmppvu );
			__m128 pvu_v_m1 = _mm_castsi128_ps( _mm_alignr_epi8( tmppvu, pvuPrev, 12 ) );
			pvuPrev = tmppvu;

			p0 = _mm_sub_ps( pvu_v, pvu_v_m1 );
			p1 = _mm_sub_ps( pvv_v, pvvm1_v );

			v_ = _mm_add_ps( v_, _mm_mul_ps( thetaV, _mm_add_ps( p0, p1 ) ) );
			_mm_store_ps( vLine + x, v_ );
		}
#else
		for ( int x = 0; x < w; ++x )
		{
			float rho = tLine[x];
			float gXAve = gxLine[x];
			float gYAve = gyLine[x];
			float initU = uLine[x];
			float initV = vLine[x];

			rho += ( initU - u0Line[x] ) * gXAve;
			rho += ( initV - v0Line[x] ) * gYAve;
			float magSq = gXAve * gXAve + gYAve * gYAve;

			float u_ = initU, v_ = initV;

			if ( rho < ( magSq * ( - lamTheta ) ) )
			{
				u_ += gXAve * lamTheta;
				v_ += gYAve * lamTheta;
			}
			else if ( rho > ( magSq * lamTheta ) )
			{
				u_ -= gXAve * lamTheta;
				v_ -= gYAve * lamTheta;
			}
			else
			{
				u_ -= rho * gXAve / magSq;
				v_ -= rho * gYAve / magSq;
			}

			float p0 = 0.F;
			if ( x > 0 )
				p0 = puuLine[x] - puuLine[x - 1];
			float p1 = puvLine[x] - puvm1L[x];

			uLine[x] = u_ + ( p0 + p1 ) * theta;

			// else will still be 0 from above
			if ( x > 0 )
				p0 = pvuLine[x] - pvuLine[x - 1];
			p1 = pvvLine[x] - pvvm1L[x];

			vLine[x] = v_ + ( p0 + p1 ) * theta;
		}
#endif
	}
}

static void
ahtvl1_updateU( plane &u, plane &v,
				const plane &puu, const plane &puv,
				const plane &pvu, const plane &pvv,
				const plane &u0, const plane &v0,
				const plane &t,
				const plane &gxAve, const plane &gyAve,
				float lamTheta, float theta )
{
	plane_buffer ub = u;
	plane_buffer vb = v;
	const_plane_buffer puub = puu;
	const_plane_buffer puvb = puv;
	const_plane_buffer pvub = pvu;
	const_plane_buffer pvvb = pvv;
	const_plane_buffer u0b = u0;
	const_plane_buffer v0b = v0;
	const_plane_buffer tb = t;
	const_plane_buffer gxb = gxAve;
	const_plane_buffer gyb = gyAve;

	threading::get().dispatch( std::bind( ahtvl1_updateU_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::ref( vb ), std::cref( puub ), std::cref( puvb ), std::cref( pvub ), std::cref( pvvb ), std::cref( u0b ), std::cref( v0b ), std::cref( tb ), std::cref( gxb ), std::cref( gyb ), lamTheta, theta ), u.y1(), u.height() );
}

#if defined(__SSE__)
GK_FORCE_INLINE __m128 rsqrtSafe( __m128 x )
{
	// rsqrt returns inf for denormals
	const __m128 zeroV = _mm_set1_ps( std::numeric_limits<float>::min() );
//	const __m128 zeroV = _mm_setzero_ps();

	__m128 y = _mm_rsqrt_ps( x );

	// avoid infinity
	__m128 notZero = _mm_cmpgt_ps( x, zeroV );
	y = _mm_and_ps( notZero, y );
#define SUPER_FAST 1
#if SUPER_FAST
	return y;
#else
# if 0
	// only 5 instructions but 2 constants that need to be loaded ...
	const __m128 three_twoV = _mm_set1_ps( 1.5F );
	const __m128 halfV = _mm_set1_ps( 0.5F );
	// newton raphson yn(3-x*yn^2)/2
	//
	__m128 halfxy2 = _mm_mul_ps( _mm_mul_ps( _mm_mul_ps( x, y ), y ), halfV );
	return _mm_mul_ps( _mm_sub_ps( three_twoV, halfxy2 ), y );
# else
	// 7 instructions but only 1 constant
	// 0.5*(y+y+y - x*y*y*y)
	// NB: do the x*y first to cancel most of the error, otherwise
	// y^3 will often end in a NaN
	const __m128 halfV = _mm_set1_ps( 0.5F );
	return _mm_mul_ps( halfV, _mm_sub_ps(
						   _mm_add_ps( y, _mm_add_ps( y, y ) ),
						   _mm_mul_ps( y, _mm_mul_ps( y, _mm_mul_ps( x, y ) ) ) ) );
# endif
#endif
}
#endif

////////////////////////////////////////

static void
ahtvl1_updatePnoedge_thread( size_t, int s, int e, plane_buffer &pu, plane_buffer &pv, const const_plane_buffer &u, float tau, float epsilon )
{
	int w = pu.width();
#if defined(__SSSE3__)
	const __m128 tauV = _mm_set1_ps( tau );
	const __m128 epsV = _mm_set1_ps( epsilon );
	const __m128 oneV = _mm_set1_ps( 1.F );
#endif
	for ( int y = s; y < e; ++y )
	{
		float * __restrict__ puLine = pu.line( y );
		float * __restrict__ pvLine = pv.line( y );
		const float * __restrict__ uLine = u.line( y );

		if ( y < pu.y2() )
		{
			const float * __restrict__ up1Line = u.line( y + 1 );
#if defined(__SSSE3__)
			int sseChunks = ( w + 3 ) / 4 - 1;
			sseChunks *= 4;
			int startOffset = sseChunks;

			__m128i curUx = _mm_load_si128( reinterpret_cast<const __m128i *>( uLine ) );
			for ( int x = 0; x < sseChunks; x += 4 )
			{
				__m128i nextUx = _mm_load_si128( reinterpret_cast<const __m128i *>( uLine + x + 4 ) );
				__m128 uX = _mm_castsi128_ps( curUx );

				__m128 outU = _mm_load_ps( puLine + x );
				__m128 outV = _mm_load_ps( pvLine + x );
				__m128 uY = _mm_load_ps( up1Line + x );

				__m128i rot = _mm_alignr_epi8( nextUx, curUx, 4 );
				__m128 uXp1 = _mm_castsi128_ps( rot );
				curUx = nextUx;

				uY = _mm_sub_ps( uY, uX );
				uX = _mm_sub_ps( uXp1, uX );

				outU = _mm_add_ps(
					outU,
					_mm_mul_ps( tauV,
								_mm_sub_ps(
									uX, _mm_mul_ps( epsV, outU ) ) ) );
				outV = _mm_add_ps(
					outV,
					_mm_mul_ps( tauV,
								_mm_sub_ps(
									uY, _mm_mul_ps( epsV, outV ) ) ) );

				__m128 scale = rsqrtSafe(
						_mm_add_ps( _mm_mul_ps( outU, outU ),
									_mm_mul_ps( outV, outV ) ) );
				scale = _mm_min_ps( scale, oneV );
				_mm_store_ps( puLine + x, _mm_mul_ps( scale, outU ) );
				_mm_store_ps( pvLine + x, _mm_mul_ps( scale, outV ) );
			}
#else
			int startOffset = 0;
#endif
			for ( int x = startOffset; x < w; ++x )
			{
				float outU = puLine[x];
				float outV = pvLine[x];
				float uX = uLine[x];
				float uY = up1Line[x] - uX;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;
				else
					uX = 0.F;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = mymaxf( 1.F, sqrtf( outU * outU + outV * outV ) );
				outU = outU / scale;
				outV = outV / scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
		else
		{
			for ( int x = 0; x < w; ++x )
			{
				float outU = puLine[x];
				float outV = pvLine[x];
				float uX = uLine[x];
				float uY = 0.F;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;
				else
					uX = 0.F;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = std::max( 1.F, sqrtf( outU * outU + outV * outV ) );
				outU = outU / scale;
				outV = outV / scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
	}
}

static void
ahtvl1_updatePedge_thread( size_t, int s, int e, plane_buffer &pu, plane_buffer &pv, const const_plane_buffer &u, const const_plane_buffer &edgeW, float tau, float epsilon )
{
	int w = pu.width();
	for ( int y = s; y < e; ++y )
	{
		float *puLine = pu.line( y );
		float *pvLine = pv.line( y );
		const float *uLine = u.line( y );
		const float *eLine = edgeW.line( y );

		if ( y < pu.y2() )
		{
			const float *up1Line = u.line( y + 1 );
			for ( int x = 0; x < w; ++x )
			{
				float outU = puLine[x];
				float outV = pvLine[x];
				float eW = eLine[x];
				float uX = uLine[x];
				float uY = up1Line[x] - uX;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;
				else
					uX = 0.F;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = eW / mymaxf( 1.F, sqrtf( outU * outU + outV * outV ) );
				outU = outU * scale;
				outV = outV * scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
		else
		{
			for ( int x = 0; x < w; ++x )
			{
				float outU = puLine[x];
				float outV = pvLine[x];
				float eW = eLine[x];
				float uX = uLine[x];
				float uY = 0.F;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;
				else
					uX = 0.F;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = eW / mymaxf( 1.F, sqrtf( outU * outU + outV * outV ) );
				outU = outU * scale;
				outV = outV * scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
	}
}

static void
ahtvl1_updateP( plane &puu, plane &puv, const plane &u, const plane &edgeW, float tau, float epsilon )
{
	plane_buffer puub = puu;
	plane_buffer puvb = puv;
	const_plane_buffer ub = u;
	const_plane_buffer eb = edgeW;

	if ( edgeW.valid() )
	{
		threading::get().dispatch( std::bind( ahtvl1_updatePedge_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( puub ), std::ref( puvb ), std::cref( ub ), std::cref( eb ), tau, epsilon ), puu.y1(), puu.height() );
	}
	else
	{
		threading::get().dispatch( std::bind( ahtvl1_updatePnoedge_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( puub ), std::ref( puvb ), std::cref( ub ), tau, epsilon ), puu.y1(), puu.height() );
	}
}
static std::string kPyramidFilter = "bilinear";
//static std::string kPyramidFilter = "bicubic";

////////////////////////////////////////

vector_field
runAHTVL1( const plane &a, const plane &b, const plane &alpha, const plane &alphaNext, const vector_field &initUV, float lambda, float theta, float epsilon, float edgePower, float edgeAlpha, int edgeBorder, int tvl1Iters, int warpIters, bool adaptiveIters, float eta )
{
	std::vector<plane> hierA = make_pyramid( a, kPyramidFilter, eta, 0, 16 );
	std::vector<plane> hierB = make_pyramid( b, kPyramidFilter, eta, 0, 16 );
	std::vector<plane> hierAlpha, hierAlphaNext;
	if ( alpha.valid() && alphaNext.valid() )
	{
		hierAlpha = make_pyramid( alpha, kPyramidFilter, eta, 0, 16 );
		hierAlphaNext = make_pyramid( alphaNext, kPyramidFilter, eta, 0, 16 );
	}

	if ( initUV.valid() )
		std::cout << "add constraint / init for initial UV passed in" << std::endl;

//	const float tau = 1.F / ( 4.F * theta + epsilon );
	std::vector<float> edgeKern;
	if ( edgeAlpha > 0.F )
	{
		TODO( "use filters when finished" );
		edgeKern.resize( 5, 0.F );
		edgeKern[0] = 1.F / 17.F;
		edgeKern[1] = 4.F / 17.F;
		edgeKern[2] = 7.F / 17.F;
		edgeKern[3] = 4.F / 17.F;
		edgeKern[4] = 1.F / 17.F;
		edgeBorder = std::max( 1, edgeBorder );
	}
	plane u, v;
	float lScale = 1.F + 1.F / static_cast<float>( hierA.size() );
	float eScale = 1.F + 1.F / static_cast<float>( hierA.size() );
//	static int oflowCount = 0;
//	int hierCount = 0;
	while ( ! hierA.empty() )
	{
		const float tau = 1.F / ( 4.F + epsilon );

		plane curA = hierA.back();
		plane curB = hierB.back();
		hierA.pop_back();
		hierB.pop_back();
		plane curAlpha, nextAlpha;
		curA.cdata();
		curB.cdata();
		if ( ! hierAlpha.empty() )
		{
			curAlpha = hierAlpha.back();
			hierAlpha.pop_back();
			curAlpha.cdata();
			nextAlpha = hierAlphaNext.back();
			hierAlphaNext.pop_back();
			nextAlpha.cdata();
		}

		if ( ! u.valid() )
		{
			u = plane( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
			v = plane( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
			memset( u.data(), 0, u.buffer_size() );
			memset( v.data(), 0, v.buffer_size() );
		}
		else
		{
			int curW = curA.width();
			int curH = curA.height();

			float scaleX = static_cast<float>( curW ) / static_cast<float>( u.width() );
			float scaleY = static_cast<float>( curH ) / static_cast<float>( u.height() );

			u = resize_bilinear( u, curW, curH ) * scaleX;
			u.data();
			v = resize_bilinear( v, curW, curH ) * scaleY;
			v.data();
		}

//		std::stringstream prefn;
//		prefn << "/local1/kthurston/oflow" << oflowCount << "_hier" << std::setw(2) << std::setfill('0') << hierCount << "_pre.exr";
//		image_buf preDbg;
//		preDbg.add_plane( u );
//		preDbg.add_plane( v );
//		preDbg.add_plane( curA );
//		preDbg.add_plane( curB );
//		debug_save_image( preDbg, prefn.str(), 0, { "R", "G", "B", "A"}, "f16" );

		plane puu = create_plane( curA.x1(), curA.y1(), curA.x2(), curA.y2(), 0.F );
		plane puv = puu.copy();
		plane pvu = puu.copy();
		plane pvv = puu.copy();

		plane edgeW;
		if ( edgeAlpha > 0.F )
			edgeW = ahtvl1_edgeWeight( curA, curAlpha, edgeKern, edgePower, edgeAlpha, edgeBorder );

		plane ax = doGradX( curA, curAlpha, lambda );
		plane ay = doGradY( curA, curAlpha, lambda );
		plane bx = doGradX( curB, nextAlpha, lambda );
		plane by = doGradY( curB, nextAlpha, lambda );

		int wI = warpIters;
		int iI = tvl1Iters;
		if ( adaptiveIters )
		{
			wI = std::max( std::min( 2, warpIters ), warpIters - static_cast<int>( hierA.size() ) );
			iI = std::max( std::min( 5, tvl1Iters ), tvl1Iters / wI );
		}

		plane t( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
		plane gxAve( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
		plane gyAve( curA.x1(), curA.y1(), curA.x2(), curA.y2() );

		std::cout << "pyramid " << curA.width() << 'x' << curA.height() << '(' << warpIters << "->" << wI << ',' << tvl1Iters << "->" << iI << ") l " << lambda << " e " << epsilon << ": ";

//		plane junkCopyDELETEME_u = u.copy();
//		plane junkCopyDELETEME_v = v.copy();
		plane u0( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
		plane v0( curA.x1(), curA.y1(), curA.x2(), curA.y2() );

		for ( int curWarp = 0; curWarp < wI; ++curWarp )
		{
			std::cout << ' ' << (curWarp + 1) << std::flush;
			ahtvl1_T( t, curB, curA, curAlpha, u0, v0, u, v );
			ahtvl1_gradAve( gxAve, bx, ax, u0, v0 );
			ahtvl1_gradAve( gyAve, by, ay, u0, v0 );
			killAlpha( gxAve, gyAve, curAlpha );
//			if ( curWarp == 0 )
//			{
//				std::stringstream initgradfn;
//				initgradfn << "/local1/kthurston/oflow" << oflowCount << "_hier" << std::setw(2) << std::setfill('0') << hierCount << "_initgrad.exr";
//				image_buf initgradDbg;
//				initgradDbg.add_plane( gxAve );
//				initgradDbg.add_plane( gyAve );
//				initgradDbg.add_plane( t );
//				initgradDbg.add_plane( u0 );
//				debug_save_image( initgradDbg, initgradfn.str(), 0, { "R", "G", "B", "A"}, "f16" );
//			}

			for ( int i = 0; i < iI; ++i )
			{
				ahtvl1_updateU( u, v, puu, puv, pvu, pvv, u0, v0, t,
								gxAve, gyAve, lambda * theta, theta );

//				if ( curWarp == 0 && i == 0 )
//				{
//					std::stringstream upd0fn;
//					upd0fn << "/local1/kthurston/oflow" << oflowCount << "_hier" << std::setw(2) << std::setfill('0') << hierCount << "_upd0.exr";
//					image_buf upd0Dbg;
//					upd0Dbg.add_plane( u );
//					upd0Dbg.add_plane( v );
//					upd0Dbg.add_plane( u0 );
//					upd0Dbg.add_plane( v0 );
//					debug_save_image( upd0Dbg, upd0fn.str(), 0, { "R", "G", "B", "A"}, "f16" );
//				}

				ahtvl1_updateP( puu, puv, u, edgeW, tau, epsilon );
				ahtvl1_updateP( pvu, pvv, v, edgeW, tau, epsilon );
			}

			killAlpha( u, v, curAlpha );
//			if ( ( curWarp + 1 ) < wI )
//			{
//				using namespace std;
//				removePeaks( u0, v0, u, v, 3 );
//				swap( u0, u );
//				swap( v0, v );
//			}
		}
		std::cout << std::endl;

//		std::stringstream postfn;
//		postfn << "/local1/kthurston/oflow" << oflowCount << "_hier" << std::setw(2) << std::setfill('0') << hierCount << "_post.exr";
//		image_buf postDbg;
//		postDbg.add_plane( u );
//		postDbg.add_plane( v );
//		postDbg.add_plane( junkCopyDELETEME_u );
//		postDbg.add_plane( junkCopyDELETEME_v );
//		debug_save_image( postDbg, postfn.str(), 0, { "R", "G", "B", "A"}, "f16" );
//		++hierCount;
		
//		if ( ! hierA.empty() )
//			removePeaks( u, v, 3 );

		if ( hierA.empty() )
			zilchAlpha( u, v, curAlpha );

		lambda *= lScale;
		epsilon *= eScale;
	}
//	++oflowCount;

	return vector_field::create( std::move( u ), std::move( v ), false );
}

////////////////////////////////////////

static void
pdtncc_gradAve_thread( size_t, int s, int e, plane_buffer &gx, const const_plane_buffer &ax, const const_plane_buffer &bx )
{
	int w = gx.width();
	int wm1 = w - 1;
	for ( int y = s; y < e; ++y )
	{
		float *gxLine = gx.line( y );
		if ( y <= gx.y1() || y >= gx.y2() )
		{
			for ( int x = 0; x < w; ++x )
				gxLine[x] = 0.F;
		}

		const float *aLine = ax.line( y );
		const float *bLine = bx.line( y );

		gxLine[0] = 0.F;
		for ( int x = 0; x < wm1; ++x )
			gxLine[x] = ( aLine[x] + bLine[x] ) * 0.5F;
		gxLine[wm1] = 0.F;
	}
}

static void
pdtncc_gradAve( plane &gxAve, const plane &ax, const plane &bx )
{
	plane_buffer gxb = gxAve;
	const_plane_buffer ab = ax;
	const_plane_buffer bb = bx;

	threading::get().dispatch( std::bind( pdtncc_gradAve_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( gxb ), std::cref( ab ), std::cref( bb ) ), bx.y1(), bx.height() );
}

////////////////////////////////////////

static void
pdtncc_dualUpdate_thread( size_t, int s, int e, plane_buffer &p1, plane_buffer &p2, plane_buffer &p3, plane_buffer &p4, plane_buffer &p5, plane_buffer &p6, const const_plane_buffer &u_, const const_plane_buffer &v_, const const_plane_buffer &w_, float sigma )
{
	int w = p1.width();
	int wm1 = w - 1;
	for ( int y = s; y < e; ++y )
	{
		float *p1L = p1.line( y );
		float *p2L = p2.line( y );
		float *p3L = p3.line( y );
		float *p4L = p4.line( y );
		float *p5L = p5.line( y );
		float *p6L = p6.line( y );
		const float *uL = u_.line( y );
		const float *vL = v_.line( y );
		const float *wL = w_.line( y );
		const float *nuL = nullptr;
		const float *nvL = nullptr;
		const float *nwL = nullptr;
		if ( y < p1.y2() )
		{
			nuL = u_.line( y + 1 );
			nvL = v_.line( y + 1 );
			nwL = w_.line( y + 1 );
		}
		for ( int x = 0; x < w; ++x )
		{
			float p1v = p1L[x];
			float p2v = p2L[x];
			float p3v = p3L[x];
			float p4v = p4L[x];
			float p5v = p5L[x];
			float p6v = p6L[x];

			float uX = uL[x];
			float uY = uX;
			float vX = vL[x];
			float vY = vX;
			float wX = wL[x];
			float wY = wX;
			if ( x < wm1 )
			{
				uX = uL[x + 1] - uX;
				vX = vL[x + 1] - vX;
				wX = wL[x + 1] - wX;
			}
			else
				uX = vX = wX = 0.0F;

			if ( nuL )
			{
				uY = nuL[x] - uY;
				vY = nvL[x] - vY;
				wY = nwL[x] - wY;
			}
			else
				uY = vY = wY = 0.F;

			p1v += sigma * uX;
			p2v += sigma * uY;
			p3v += sigma * vX;
			p4v += sigma * vY;
			p5v += sigma * wX;
			p6v += sigma * wY;

			float reproj = 1.F / mymaxf( 1.F, sqrtf( p1v * p1v + p2v * p2v + p3v * p3v + p4v * p4v ) );
			p1L[x] = p1v * reproj;
			p2L[x] = p2v * reproj;
			p3L[x] = p3v * reproj;
			p4L[x] = p4v * reproj;

			reproj = 1.F / mymaxf( 1.F, sqrtf( p5v * p5v + p6v * p6v ) );
			p5L[x] = p5v * reproj;
			p6L[x] = p6v * reproj;
		}
	}
}

////////////////////////////////////////

static void
pdtncc_dualUpdateAlpha_thread( size_t, int s, int e, plane_buffer &p1, plane_buffer &p2, plane_buffer &p3, plane_buffer &p4, plane_buffer &p5, plane_buffer &p6, const const_plane_buffer &u_, const const_plane_buffer &v_, const const_plane_buffer &w_, const const_plane_buffer &alpha, float sigma )
{
	int w = p1.width();
	int wm1 = w - 1;
	for ( int y = s; y < e; ++y )
	{
		float *p1L = p1.line( y );
		float *p2L = p2.line( y );
		float *p3L = p3.line( y );
		float *p4L = p4.line( y );
		float *p5L = p5.line( y );
		float *p6L = p6.line( y );
		const float *uL = u_.line( y );
		const float *vL = v_.line( y );
		const float *wL = w_.line( y );
		const float *aL = alpha.line( y );
		const float *nuL = nullptr;
		const float *nvL = nullptr;
		const float *nwL = nullptr;
//		const float *naL = nullptr;
		if ( y < p1.y2() )
		{
			nuL = u_.line( y + 1 );
			nvL = v_.line( y + 1 );
			nwL = w_.line( y + 1 );
//			naL = alpha.line( y + 1 );
		}
		for ( int x = 0; x < w; ++x )
		{
			float curSig = aL[x] * sigma;
			float p1v = p1L[x];
			float p2v = p2L[x];
			float p3v = p3L[x];
			float p4v = p4L[x];
			float p5v = p5L[x];
			float p6v = p6L[x];

			float uX = uL[x];
			float uY = uX;
			float vX = vL[x];
			float vY = vX;
			float wX = wL[x];
			float wY = wX;
			if ( x < wm1 )
			{
				uX = uL[x + 1] - uX;
				vX = vL[x + 1] - vX;
				wX = wL[x + 1] - wX;
			}
			else
				uX = vX = wX = 0.0F;

			if ( nuL )
			{
				uY = nuL[x] - uY;
				vY = nvL[x] - vY;
				wY = nwL[x] - wY;
			}
			else
				uY = vY = wY = 0.F;

			p1v += curSig * uX;
			p2v += curSig * uY;
			p3v += curSig * vX;
			p4v += curSig * vY;
			p5v += curSig * wX;
			p6v += curSig * wY;

			float reproj = 1.F / mymaxf( 1.F, sqrtf( p1v * p1v + p2v * p2v + p3v * p3v + p4v * p4v ) );
			p1L[x] = p1v * reproj;
			p2L[x] = p2v * reproj;
			p3L[x] = p3v * reproj;
			p4L[x] = p4v * reproj;

			reproj = 1.F / mymaxf( 1.F, sqrtf( p5v * p5v + p6v * p6v ) );
			p5L[x] = p5v * reproj;
			p6L[x] = p6v * reproj;
		}
	}
}

static void
pdtncc_dualUpdate( plane &p0, plane &p1, plane &p2, plane &p3, plane &p4, plane &p5, const plane &u, const plane &v, const plane &w, const plane &alpha, float sigma )
{
	plane_buffer p0b = p0;
	plane_buffer p1b = p1;
	plane_buffer p2b = p2;
	plane_buffer p3b = p3;
	plane_buffer p4b = p4;
	plane_buffer p5b = p5;
	const_plane_buffer ub = u;
	const_plane_buffer vb = v;
	const_plane_buffer wb = w;

	if ( alpha.valid() )
	{
		const_plane_buffer ab = alpha;

		threading::get().dispatch( std::bind( pdtncc_dualUpdateAlpha_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( p0b ), std::ref( p1b ), std::ref( p2b ), std::ref( p3b ), std::ref( p4b ), std::ref( p5b ), std::cref( ub ), std::cref( vb ), std::cref( wb ), std::cref( ab ), sigma ), u.y1(), u.height() );
	}
	else
	{
		threading::get().dispatch( std::bind( pdtncc_dualUpdate_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( p0b ), std::ref( p1b ), std::ref( p2b ), std::ref( p3b ), std::ref( p4b ), std::ref( p5b ), std::cref( ub ), std::cref( vb ), std::cref( wb ), sigma ), u.y1(), u.height() );
	}
}

////////////////////////////////////////

static void
pdtncc_primalRho2_thread( size_t, int s, int e,
	plane_buffer &u_, plane_buffer &u,
	plane_buffer &v_, plane_buffer &v,
	plane_buffer &w_, plane_buffer &w,
	const const_plane_buffer &p0,
	const const_plane_buffer &p1,
	const const_plane_buffer &p2,
	const const_plane_buffer &p3,
	const const_plane_buffer &p4,
	const const_plane_buffer &p5,
	const const_plane_buffer &dT,
	const const_plane_buffer &u0,
	const const_plane_buffer &v0,
	const const_plane_buffer &dX,
	const const_plane_buffer &dY,
	float tau, float lambda, float gamv )
{
	int wNS = u.width();
	const float eps = 1e-9F;

	for ( int y = s; y < e; ++y )
	{
		float *u_L = u_.line( y );
		float *uL = u.line( y );
		float *v_L = v_.line( y );
		float *vL = v.line( y );
		float *w_L = w_.line( y );
		float *wL = w.line( y );
		const float *p0L = p0.line( y );
		const float *p1L = p1.line( y );
		const float *p2L = p2.line( y );
		const float *p3L = p3.line( y );
		const float *p4L = p4.line( y );
		const float *p5L = p5.line( y );
		const float *dTL = dT.line( y );
		const float *u0L = u0.line( y );
		const float *v0L = v0.line( y );
		const float *dXL = dX.line( y );
		const float *dYL = dY.line( y );
		const float *pp1L = nullptr;
		const float *pp3L = nullptr;
		const float *pp5L = nullptr;

		if ( y > p1.y1() )
		{
			pp1L = p1.line( y - 1 );
			pp3L = p3.line( y - 1 );
			pp5L = p5.line( y - 1 );
		}
		for ( int x = 0; x < wNS; ++x )
		{
			float rho = dTL[x];
			float uV = uL[x];
			float vV = vL[x];
			float wV = wL[x];
			float u_v = uV;
			float v_v = vV;
			float w_v = wV;

			float p0V = p0L[x];
			float p1V = p1L[x];
			float p2V = p2L[x];
			float p3V = p3L[x];
			float p4V = p4L[x];
			float p5V = p5L[x];

			if ( x > 0 )
			{
				p0V -= p0L[x - 1];
				p2V -= p2L[x - 1];
				p4V -= p4L[x - 1];
			}

			if ( pp1L )
			{
				p1V -= pp1L[x];
				p3V -= pp3L[x];
				p5V -= pp5L[x];
			}

			uV += tau * ( p0V + p1V );
			vV += tau * ( p2V + p3V );
			wV += tau * ( p4V + p5V );

			float dXV = dXL[x];
			float dYV = dYL[x];

			rho += ( uV - u0L[x] ) * dXV + ( vV - v0L[x] ) * dYV + wV * gamv;

			float gradSqr = mymaxf( eps, dXV * dXV + dYV * dYV + gamv * gamv );
			float testVal = gradSqr * tau * lambda;

			if ( rho < - testVal )
			{
				uV += dXV * tau * lambda;
				vV += dYV * tau * lambda;
				wV += gamv * tau * lambda;
			}
			else if ( rho > testVal )
			{
				uV -= dXV * tau * lambda;
				vV -= dYV * tau * lambda;
				wV -= gamv * tau * lambda;
			}
			else
			{
				uV -= rho * dXV / gradSqr;
				vV -= rho * dYV / gradSqr;
				wV -= rho * gamv / gradSqr;
			}

			uL[x] = uV;
			u_L[x] = 2.F * uV - u_v;
			vL[x] = vV;
			v_L[x] = 2.F * vV - v_v;
			wL[x] = wV;
			w_L[x] = 2.F * wV - w_v;
		}
	}
}

////////////////////////////////////////

static void
pdtncc_primalRho2alpha_thread( size_t, int s, int e,
	plane_buffer &u_, plane_buffer &u,
	plane_buffer &v_, plane_buffer &v,
	plane_buffer &w_, plane_buffer &w,
	const const_plane_buffer &p0,
	const const_plane_buffer &p1,
	const const_plane_buffer &p2,
	const const_plane_buffer &p3,
	const const_plane_buffer &p4,
	const const_plane_buffer &p5,
	const const_plane_buffer &dT,
	const const_plane_buffer &u0,
	const const_plane_buffer &v0,
	const const_plane_buffer &dX,
	const const_plane_buffer &dY,
	const const_plane_buffer &alpha,
	float tau, float lambda, float gamv )
{
	int wNS = u.width();

	for ( int y = s; y < e; ++y )
	{
		float *u_L = u_.line( y );
		float *uL = u.line( y );
		float *v_L = v_.line( y );
		float *vL = v.line( y );
		float *w_L = w_.line( y );
		float *wL = w.line( y );
		const float *p0L = p0.line( y );
		const float *p1L = p1.line( y );
		const float *p2L = p2.line( y );
		const float *p3L = p3.line( y );
		const float *p4L = p4.line( y );
		const float *p5L = p5.line( y );
		const float *dTL = dT.line( y );
		const float *u0L = u0.line( y );
		const float *v0L = v0.line( y );
		const float *dXL = dX.line( y );
		const float *dYL = dY.line( y );
		const float *aL = alpha.line( y );
		const float *pp1L = nullptr;
		const float *pp3L = nullptr;
		const float *pp5L = nullptr;
//		const float *paL = nullptr;

		if ( y > p1.y1() )
		{
			pp1L = p1.line( y - 1 );
			pp3L = p3.line( y - 1 );
			pp5L = p5.line( y - 1 );
//			paL = alpha.line( y - 1 );
		}

		for ( int x = 0; x < wNS; ++x )
		{
			float rho = dTL[x];
			float uV = uL[x];
			float vV = vL[x];
			float wV = wL[x];
			float u_v = uV;
			float v_v = vV;
			float w_v = wV;

			float p0V = p0L[x];
			float p1V = p1L[x];
			float p2V = p2L[x];
			float p3V = p3L[x];
			float p4V = p4L[x];
			float p5V = p5L[x];

			if ( x > 0 )// && aL[x - 1] > 0.F )
			{
				p0V -= p0L[x - 1];
				p2V -= p2L[x - 1];
				p4V -= p4L[x - 1];
			}

			if ( pp1L )//&& paL[x] > 0.F )
			{
				p1V -= pp1L[x];
				p3V -= pp3L[x];
				p5V -= pp5L[x];
			}

			uV += tau * ( p0V + p1V );
			vV += tau * ( p2V + p3V );
			wV += tau * ( p4V + p5V );

//			float dXV = std::min( 1.F, std::max( -1.F, dXL[x] * aL[x] ) );
//			float dYV = std::min( 1.F, std::max( -1.F, dYL[x] * aL[x] ) );
			float dXV = dXL[x];
			float dYV = dYL[x];
			if ( aL[x] <= 0.F )
			{
				dXV = 0.F;
				dYV = 0.F;
			}

			rho += ( uV - u0L[x] ) * dXV + ( vV - v0L[x] ) * dYV + wV * gamv;

			float gradSqr = dXV * dXV + dYV * dYV + gamv * gamv;
			float testVal = gradSqr * tau * lambda;

			if ( rho < - testVal )
			{
				uV += dXV * tau * lambda;
				vV += dYV * tau * lambda;
				wV += gamv * tau * lambda;
			}
			else if ( rho > testVal )
			{
				uV -= dXV * tau * lambda;
				vV -= dYV * tau * lambda;
				wV -= gamv * tau * lambda;
			}
			else
			{
				uV -= rho * dXV / gradSqr;
				vV -= rho * dYV / gradSqr;
				wV -= rho * gamv / gradSqr;
			}

			uL[x] = uV;
			u_L[x] = 2.F * uV - u_v;
			vL[x] = vV;
			v_L[x] = 2.F * vV - v_v;
			wL[x] = wV;
			w_L[x] = 2.F * wV - w_v;
		}
	}
}

static void
pdtncc_primalRho2( plane &u_, plane &u, plane &v_, plane &v, plane &w_, plane &w, const plane &p0, const plane &p1, const plane &p2, const plane &p3, const plane &p4, const plane &p5, const plane &dT, const plane &u0, const plane &v0, const plane &dX, const plane &dY, const plane &alpha, float tau, float lambda, float gamv )
{
	plane_buffer u_b = u_;
	plane_buffer ub = u;
	plane_buffer v_b = v_;
	plane_buffer vb = v;
	plane_buffer w_b = w_;
	plane_buffer wb = w;
	const_plane_buffer p0b = p0;
	const_plane_buffer p1b = p1;
	const_plane_buffer p2b = p2;
	const_plane_buffer p3b = p3;
	const_plane_buffer p4b = p4;
	const_plane_buffer p5b = p5;
	const_plane_buffer dTb = dT;
	const_plane_buffer u0b = u0;
	const_plane_buffer v0b = v0;
	const_plane_buffer dXb = dX;
	const_plane_buffer dYb = dY;
	const double eps = 1e-9;

	gamv = static_cast<float>( sqrt( fmax( static_cast<double>( gamv ) * static_cast<double>( gamv ), eps ) ) );

	if ( alpha.valid() )
	{
		const_plane_buffer alpB = alpha;

		threading::get().dispatch( std::bind( pdtncc_primalRho2alpha_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref(u_b), std::ref(ub), std::ref(v_b), std::ref(vb), std::ref(w_b), std::ref(wb), std::cref( p0b ), std::cref( p1b ), std::cref( p2b ), std::cref( p3b ), std::cref( p4b ), std::cref( p5b ), std::cref( dTb ), std::cref( u0b ), std::cref( v0b ), std::cref( dXb ), std::cref( dYb ), std::cref( alpB ), tau, lambda, gamv ), u.y1(), u.height() );
	}
	else
	{
		threading::get().dispatch( std::bind( pdtncc_primalRho2_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref(u_b), std::ref(ub), std::ref(v_b), std::ref(vb), std::ref(w_b), std::ref(wb), std::cref( p0b ), std::cref( p1b ), std::cref( p2b ), std::cref( p3b ), std::cref( p4b ), std::cref( p5b ), std::cref( dTb ), std::cref( u0b ), std::cref( v0b ), std::cref( dXb ), std::cref( dYb ), tau, lambda, gamv ), u.y1(), u.height() );
	}
}

////////////////////////////////////////

static vector_field
runPD( const plane &a, const plane &b, const plane &alpha, const plane &alphaNext, const vector_field &initUV, float lambda, float gamv, int innerIters, int warpIters, bool adaptiveIters, float eta )
{
	TODO( "Add ability to pass in initial u, v, and alpha channel to control diffusion" );
	static bool first = true;
	if ( first )
	{
		std::cout << "switch resize to bspline when ready" << std::endl;
		first = false;
	}

	std::vector<plane> hierA = make_pyramid( a, kPyramidFilter, eta, 0, 16 );
	std::vector<plane> hierB = make_pyramid( b, kPyramidFilter, eta, 0, 16 );
	std::vector<plane> hierAlpha, hierAlphaNext;
	if ( alpha.valid() && alphaNext.valid() )
	{
		hierAlpha = make_pyramid( alpha, kPyramidFilter, eta, 0, 16 );
		hierAlphaNext = make_pyramid( alphaNext, kPyramidFilter, eta, 0, 16 );
	}

	if ( initUV.valid() )
	{
		std::cout << "Implement input UV constraint / mapping options" << std::endl;
	}

	float tau = 1.0 / ::sqrt( 8.0 );
	float sigma = tau; // 1.F / L

	plane u, v;
	plane lumW;
	plane p[6];
	float lScale = 1.F + 1.F / static_cast<float>( hierA.size() );
	while ( ! hierA.empty() )
	{
		plane curA = hierA.back();
		plane curB = hierB.back();
		hierA.pop_back();
		hierB.pop_back();
		plane curAlpha, nextAlpha;
		if ( ! hierAlpha.empty() )
		{
			curAlpha = hierAlpha.back();
			hierAlpha.pop_back();
			nextAlpha = hierAlphaNext.back();
			hierAlphaNext.pop_back();
		}

		if ( ! u.valid() )
		{
			u = create_plane( curA.x1(), curA.y1(), curA.x2(), curA.y2(), 0.F );
			v = u.copy();
			lumW = u.copy();
			for ( int i = 0; i != 6; ++i )
				p[i] = u.copy();
		}
		else
		{
			int curW = curA.width();
			int curH = curA.height();

			float scaleX = static_cast<float>( curW ) / static_cast<float>( u.width() );
			float scaleY = static_cast<float>( curH ) / static_cast<float>( u.height() );

			u = resize_bilinear( u, curW, curH ) * scaleX;
			v = resize_bilinear( v, curW, curH ) * scaleY;

			lumW = resize_bilinear( lumW, curW, curH );
			for ( int i = 0; i != 6; ++i )
				p[i] = resize_bilinear( p[i], curW, curH );
		}

		plane u_ = u.copy();
		plane v_ = v.copy();
		plane lumw_ = lumW.copy();

		plane dX, dY;
		plane aX, aY;

		bool splitGrads = true;
//		if ( hierA.empty() )
//		{
//			dX = noise_gradient_horiz5( curA );
//			dirichlet( dX );
//			dY = noise_gradient_vert5( curA );
//			dirichlet( dY );
//			splitGrads = false;
//		}
//		else
		{
			dX = plane( curA.x1(), curA.y1(), curA.x2(), curA.y2() );
			dY = plane( curA.x1(), curA.y1(), curA.x2(), curA.y2() );

			aX = doGradX( curA, curAlpha, lambda );
			aY = doGradY( curA, curAlpha, lambda );
		}

		int wI = std::max( 1, warpIters );
		int iI = std::max( 1, innerIters );
		if ( adaptiveIters )
		{
			wI = std::max( std::min( 2, warpIters ), warpIters - static_cast<int>( hierA.size() ) );
			iI = std::max( std::min( 5, innerIters ), innerIters / wI );
		}

		std::cout << "pyramid " << curA.width() << 'x' << curA.height() << '(' << warpIters << "->" << wI << ',' << innerIters << "->" << iI << ") l " << lambda << ": ";
		for ( int w = 0; w != wI; ++w )
		{
			std::cout << ' ' << (w + 1) << std::flush;
			plane u0 = u.copy();
			plane v0 = v.copy();

			vector_field curVecs = vector_field::create( u, v, false );
			plane dT = warp_bilinear( curB, curVecs );
			if ( splitGrads )
			{
				plane bX, bY;
				plane warpAlpha;
				if ( nextAlpha.valid() )
					warpAlpha = warp_bilinear( nextAlpha, curVecs );

				bX = doGradX( dT, warpAlpha, lambda );
				bY = doGradY( dT, warpAlpha, lambda );

				pdtncc_gradAve( dX, aX, bX );
				pdtncc_gradAve( dY, aY, bY );
			}
			curVecs = vector_field();

			dT = dT - curA;
			dirichlet( dT );

			for ( int i = 0; i != iI; ++i )
			{
				// update the dual variables
				pdtncc_dualUpdate( p[0], p[1], p[2], p[3], p[4], p[5], u_, v_, lumw_, curAlpha, sigma );

				// update the primal
				pdtncc_primalRho2( u_, u, v_, v, lumw_, lumW, p[0], p[1], p[2], p[3], p[4], p[5], dT, u0, v0, dX, dY, curAlpha, tau, lambda, gamv );
			}
			killAlpha( u, v, curAlpha );
			if ( ( w + 1 ) < wI )
				removePeaks( u, v, 3 );
		}
		std::cout << std::endl;

		if ( ! hierA.empty() )
			removePeaks( u, v, 3 );
		else
			zilchAlpha( u, v, curAlpha );
		lambda *= lScale;
	}
	
	return vector_field::create( std::move( u ), std::move( v ), false );
}

////////////////////////////////////////

static vector_field
runZilch( const vector_field &a, const plane &alpha )
{
	plane newU = a.u().copy();
	plane newV = a.v().copy();
	zilchAlpha( newU, newV, alpha );
	return vector_field::create( std::move( newU ), std::move( newV ), a.is_absolute() );
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

vector_field
oflow_ahtvl1( const plane &a, const plane &b, const plane &alpha, const plane &alphaNext, const vector_field &initUV, float lambda, float theta, float epsilon, float edgePower, float edgeAlpha, int edgeBorder, int tvl1Iters, int warpIters, bool adaptiveIters, float eta )
{
	precondition( a.dims() == b.dims(), "oflow_ahtvl1 must have a & b of same size, received a {0} b {1}", a.dims(), b.dims() );
	engine::dimensions d = a.dims();
	d.planes = 2;

	return vector_field( false, "p.oflow_ahtvl1", d, a, b, alpha, alphaNext, initUV, lambda, theta, epsilon, edgePower, edgeAlpha, edgeBorder, tvl1Iters, warpIters, adaptiveIters, eta );
}

////////////////////////////////////////

vector_field
oflow_primaldual( const plane &a, const plane &b, const plane &alpha, const plane &alphaNext, const vector_field &initUV, float lambda, float sigma, int innerIters, int warpIters, bool adaptiveIters, float eta )
{
	precondition( a.dims() == b.dims(), "oflow_primaldual must have a & b of same size, received a {0} b {1}", a.dims(), b.dims() );
	engine::dimensions d = a.dims();
	d.planes = 2;

	return vector_field( false, "p.oflow_primaldual", d, a, b, alpha, alphaNext, initUV, lambda, sigma, innerIters, warpIters, adaptiveIters, eta );
}

////////////////////////////////////////

vector_field
oflow_zilch_alpha( const vector_field &a, const plane &alpha )
{
	if ( ! alpha.valid() )
		return a;

	precondition( a.u().dims() == alpha.dims(), "zilch_alpha must have an alpha channel of same size as vectors, received a {0} alpha {1}", a.dims(), alpha.dims() );

	return vector_field( a.is_absolute(), "v.zilch_alpha", a.dims(), a, alpha );
}

////////////////////////////////////////

void
add_oflow( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.oflow_ahtvl1", runAHTVL1, op::threaded ) );
	r.add( op( "p.oflow_primaldual", runPD, op::threaded ) );
	r.add( op( "v.zilch_alpha", runZilch, op::threaded ) );
}

////////////////////////////////////////

} // namespace image

