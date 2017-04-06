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
#include <cmath>

////////////////////////////////////////

namespace
{
using namespace image;

static plane extract_u( const vector_field &v )
{
	return v.u();
}

////////////////////////////////////////

static plane extract_v( const vector_field &v )
{
	return v.v();
}

////////////////////////////////////////

static void
ahtvl1_edgeWeight_thread( size_t, int s, int e, plane_buffer &ew, const const_plane_buffer &eb, float ePower, float eWeight, int border )
{
	int w = eb.width();
	int h = eb.height();
	int exm1 = w - border - 1;
	const float eps = 0.000001F;
	for ( int y = s; y < e; ++y )
	{
		float *ewLine = ew.line( y );
		if ( y < border || y > ( h - border - 1 ) )
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
			float gxv = epx - emx;
			float gyv = epy - emy;
			float norm = std::sqrt( gxv * gxv + gyv * gyv );
			ewLine[x] = std::max( eps, expf( eWeight * powf( norm, ePower ) ) );
		}

		for ( int x = exm1 + 1; x < w; ++x )
			ewLine[x] = 1.F;
	}
}

static plane
ahtvl1_edgeWeight( const plane &curA, const std::vector<float> &edgeKern, float edgePower, float edgeAlpha, int edgeBorder )
{
	plane ret( curA.width(), curA.height() );
	plane ba = separable_convolve( curA, edgeKern );
	plane_buffer eb = ret;
	const_plane_buffer bab = ba;

	threading::get().dispatch( std::bind( ahtvl1_edgeWeight_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( eb ), std::cref( bab ), edgePower, - edgeAlpha, edgeBorder ), 0, curA.height() );

	return ret;
}

////////////////////////////////////////

static void
peakThread( size_t, int s, int e, plane_buffer &u, const plane_buffer &mu, int r )
{
	int w = u.width();
	float norm = 2.F * static_cast<float>( r ) + 1.F;
	norm = 1.F / ( norm * norm );
	for ( int hy = s; hy < e; ++hy )
	{
		float *uLine = u.line( hy );
		const float *muLine = mu.line( hy );
		for ( int hx = 0; hx < w; ++hx )
		{
			float uV = uLine[hx];
			float muV = muLine[hx];
			float sumMU = 0.F;
			for ( int y = -r; y <= r; ++y )
			{
				for ( int x = -r; x <= r; ++x )
				{
					sumMU += get_hold( mu, hx + x, hy + y );
				}
			}
			float aveMU = sumMU * norm;
			if ( std::abs( uV - muV ) > aveMU )
				uV = muV;
		}
	}
}

static void
removePeaks( plane &u, plane &v, int r )
{
	{
		plane mu = median( u, 3 );
		plane_buffer mub = mu;
		plane_buffer ub = u;
		threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::cref( mub ), r ), 0, u.height() );
	}

	plane mv = median( v, 3 );
	plane_buffer mvb = mv;
	plane_buffer vb = v;
	threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( vb ), std::cref( mvb ), r ), 0, v.height() );
}

////////////////////////////////////////

static void
ahtvl1_T_thread( size_t, int s, int e, plane_buffer &t, const const_plane_buffer &b, const const_plane_buffer &a, const const_plane_buffer &u, const const_plane_buffer &v )
{
	int w = t.width();
	for ( int y = s; y < e; ++y )
	{
		float *tLine = t.line( y );
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		const float *aLine = a.line( y );

		float curY = static_cast<float>( y );
		for ( int x = 0; x < w; ++x )
		{
			float warpX = uLine[x];
			float warpY = vLine[x];

			int inCheckX = x + static_cast<int>( warpX );
			int inCheckY = y + static_cast<int>( warpY );
			warpX += static_cast<float>( x );
			warpY += curY;

			float bW = bilinear_hold( b, warpX, warpY );
			float aW = aLine[x];
			float out = 0.F;
			if ( t.in_bounds( inCheckX, inCheckY ) )
				out = bW - aW;

			tLine[x] = out;
		}
	}
}

static void
ahtvl1_T( plane &t, const plane &curB, const plane &curA, const plane &u0, const plane &v0 )
{
	plane_buffer tb = t;
	const_plane_buffer bb = curB;
	const_plane_buffer ab = curA;
	const_plane_buffer ub = u0;
	const_plane_buffer vb = v0;

	threading::get().dispatch( std::bind( ahtvl1_T_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( tb ), std::cref( bb ), std::cref( ab ), std::cref( ub ), std::cref( vb ) ), 0, curB.height() );
}

////////////////////////////////////////

static void
ahtvl1_gradAve_thread( size_t, int s, int e, plane_buffer &gx, const const_plane_buffer &bx, const const_plane_buffer &ax, const const_plane_buffer &u, const const_plane_buffer &v )
{
	int w = gx.width();
	for ( int y = s; y < e; ++y )
	{
		float *gxLine = gx.line( y );
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		const float *aLine = ax.line( y );

		float curY = static_cast<float>( y );
		for ( int x = 0; x < w; ++x )
		{
			float warpX = uLine[x];
			float warpY = vLine[x];
			int inCheckX = x + static_cast<int>( warpX );
			int inCheckY = y + static_cast<int>( warpY );
			warpX += static_cast<float>( x );
			warpY += curY;

			float bW = bilinear_hold( bx, warpX, warpY );
			float aW = aLine[x];
			float out = 0.F;
			if ( gx.in_bounds( inCheckX, inCheckY ) )
				out = ( aW + bW ) * 0.5F;
			
			gxLine[x] = out;
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

	threading::get().dispatch( std::bind( ahtvl1_gradAve_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( gxb ), std::cref( bb ), std::cref( ab ), std::cref( ub ), std::cref( vb ) ), 0, bx.height() );
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
	const float eps = 0.000001F;
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		const float *tLine = t.line( y );
		const float *gxLine = gx.line( y );
		const float *gyLine = gy.line( y );
		const float *u0Line = u0.line( y );
		const float *v0Line = v0.line( y );

		const float *puuLine = puu.line( y );
		const float *puvLine = puv.line( y );
		const float *pvuLine = pvu.line( y );
		const float *pvvLine = pvv.line( y );
		const float *puvm1L = nullptr;
		const float *pvvm1L = nullptr;
		if ( y > 0 )
		{
			puvm1L = puv.line( y - 1 );
			pvvm1L = pvv.line( y - 1 );
		}
		for ( int x = 0; x < w; ++x )
		{
			float rho = tLine[x];
			float gXAve = gxLine[x];
			float gYAve = gyLine[x];
			float initU = uLine[x];
			float initV = vLine[x];

			rho += ( initU - u0Line[x] ) * gXAve;
			rho += ( initV - v0Line[x] ) * gYAve;
			float magSq = std::max( eps, gXAve * gXAve + gYAve * gYAve );

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

			float p0 = puuLine[x];
			float p1 = puvLine[x];
			if ( x > 0 )
				p0 -= puuLine[x - 1];
			if ( puvm1L )
				p1 -= puvm1L[x];
			uLine[x] = u_ + ( p0 + p1 ) * theta;

			p0 = pvuLine[x];
			p1 = pvvLine[x];
			if ( x > 0 )
				p0 -= pvuLine[x - 1];
			if ( pvvm1L )
				p1 -= pvvm1L[x];
			vLine[x] = v_ + ( p0 + p1 ) * theta;
		}
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

	threading::get().dispatch( std::bind( ahtvl1_updateU_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::ref( vb ), std::cref( puub ), std::cref( puvb ), std::cref( pvub ), std::cref( pvvb ), std::cref( u0b ), std::cref( v0b ), std::cref( tb ), std::cref( gxb ), std::cref( gyb ), lamTheta, theta ), 0, u.height() );
}

////////////////////////////////////////

static void
ahtvl1_updatePnoedge_thread( size_t, int s, int e, plane_buffer &pu, plane_buffer &pv, const const_plane_buffer &u, float tau, float epsilon )
{
	int w = pu.width();
	int h = pu.height();
	for ( int y = s; y < e; ++y )
	{
		float *puLine = pu.line( y );
		float *pvLine = pv.line( y );
		const float *uLine = u.line( y );
		if ( y < (h - 1) )
		{
			const float *up1Line = u.line( y + 1 );
			for ( int x = 0; x < w; ++x )
			{
				float outU = puLine[x];
				float outV = pvLine[x];
				float uX = uLine[x];
				float uY = up1Line[x] - uX;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = std::max( 1.F, std::sqrt( outU * outU + outV * outV ) );
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

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = std::max( 1.F, std::sqrt( outU * outU + outV * outV ) );
				outU = outU / scale;
				outV = outV / scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
	}
}

static void
ahtvl1_updatePnoedge( plane &puu, plane &puv, const plane &u, float tau, float epsilon )
{
	plane_buffer puub = puu;
	plane_buffer puvb = puv;
	const_plane_buffer ub = u;

	threading::get().dispatch( std::bind( ahtvl1_updatePnoedge_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( puub ), std::ref( puvb ), std::cref( ub ), tau, epsilon ), 0, puu.height() );
}

static void
ahtvl1_updatePedge_thread( size_t, int s, int e, plane_buffer &pu, plane_buffer &pv, const const_plane_buffer &u, const const_plane_buffer &edgeW, float tau, float epsilon )
{
	int w = pu.width();
	int h = pu.height();
	for ( int y = s; y < e; ++y )
	{
		float *puLine = pu.line( y );
		float *pvLine = pv.line( y );
		const float *uLine = u.line( y );
		const float *eLine = edgeW.line( y );
		if ( y < (h - 1) )
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

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = eW / std::max( 1.F, std::sqrt( outU * outU + outV * outV ) );
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

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = eW / std::max( 1.F, std::sqrt( outU * outU + outV * outV ) );
				outU = outU * scale;
				outV = outV * scale;
				puLine[x] = outU;
				pvLine[x] = outV;
			}
		}
	}
}

static void
ahtvl1_updatePedge( plane &puu, plane &puv, const plane &u, const plane &edgeW, float tau, float epsilon )
{
	plane_buffer puub = puu;
	plane_buffer puvb = puv;
	const_plane_buffer ub = u;
	const_plane_buffer eb = edgeW;

	threading::get().dispatch( std::bind( ahtvl1_updatePedge_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( puub ), std::ref( puvb ), std::cref( ub ), std::cref( eb ), tau, epsilon ), 0, puu.height() );
}

////////////////////////////////////////

vector_field
runAHTVL1( const plane &a, const plane &b, float lambda, float theta, float epsilon, float edgePower, float edgeAlpha, int edgeBorder, int tvl1Iters, int warpIters, float eta )
{
	std::vector<plane> hierA = make_pyramid( a, "bicubic", eta, 0, 16 );
	std::vector<plane> hierB = make_pyramid( b, "bicubic", eta, 0, 16 );

	const float tau = 1.F / ( 4.F * theta + epsilon );
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
	}
	plane u, v;
//	plane puu, puv, pvu, pvv;
	while ( ! hierA.empty() )
	{
		plane curA = hierA.back();
		plane curB = hierB.back();
		int curW = curA.width();
		int curH = curA.height();
		hierA.pop_back();
		hierB.pop_back();

		if ( ! u.valid() )
		{
			u = create_plane( curW, curH, 0.F );
			v = create_plane( curW, curH, 0.F );
//			puu = create_plane( curW, curH, 0.F );
//			puv = create_plane( curW, curH, 0.F );
//			pvu = create_plane( curW, curH, 0.F );
//			pvv = create_plane( curW, curH, 0.F );
		}
		else
		{
			float scaleX = static_cast<float>( curW ) / static_cast<float>( u.width() );
			float scaleY = static_cast<float>( curH ) / static_cast<float>( u.height() );

			u = resize_bicubic( u, curW, curH ) * scaleX;
			v = resize_bicubic( v, curW, curH ) * scaleY;

//			puu = resize_bicubic( puu, curW, curH );
//			puv = resize_bicubic( puv, curW, curH );
//			pvu = resize_bicubic( pvu, curW, curH );
//			pvv = resize_bicubic( pvv, curW, curH );
		}

		plane puu = create_plane( curW, curH, 0.F );
		plane puv = create_plane( curW, curH, 0.F );
		plane pvu = create_plane( curW, curH, 0.F );
		plane pvv = create_plane( curW, curH, 0.F );

		plane edgeW;
		if ( edgeAlpha > 0.F )
			edgeW = ahtvl1_edgeWeight( curA, edgeKern, edgePower, edgeAlpha, edgeBorder );

		plane ax = noise_gradient_horiz5( curA );
		plane ay = noise_gradient_vert5( curA );
		plane bx = noise_gradient_horiz5( curB );
		plane by = noise_gradient_vert5( curB );

		int wI = std::max( 1, warpIters - static_cast<int>( hierA.size() ) );
		int iI = std::max( 1, tvl1Iters / wI );

		plane t( curW, curH );
		plane gxAve( curW, curH );
		plane gyAve( curW, curH );

		for ( int curWarp = 0; curWarp < wI; ++curWarp )
		{
			plane u0 = u.copy();
			plane v0 = v.copy();

			ahtvl1_T( t, curB, curA, u0, v0 );
			ahtvl1_gradAve( gxAve, bx, ax, u0, v0 );
			ahtvl1_gradAve( gyAve, by, ay, u0, v0 );

			for ( int i = 0; i < iI; ++i )
			{
				ahtvl1_updateU( u, v, puu, puv, pvu, pvv, u0, v0, t,
								gxAve, gyAve, lambda * theta, theta );

				if ( edgeW.valid() )
				{
					ahtvl1_updatePedge( puu, puv, u, edgeW, tau, epsilon );
					ahtvl1_updatePedge( pvu, pvv, v, edgeW, tau, epsilon );
				}
				else
				{
					ahtvl1_updatePnoedge( puu, puv, u, tau, epsilon );
					ahtvl1_updatePnoedge( pvu, pvv, v, tau, epsilon );
				}
			}
		}

		removePeaks( u, v, 2 );
	}

	return vector_field::create( std::move( u ), std::move( v ), false );
}

////////////////////////////////////////

static void
pdtncc_gradAve_thread( size_t, int s, int e, plane_buffer &gx, const const_plane_buffer &ax, const const_plane_buffer &bx )
{
	int w = gx.width();
	int h = gx.height();
	int wm1 = w - 1;
	for ( int y = s; y < e; ++y )
	{
		float *gxLine = gx.line( y );
		if ( y < 1 || y >= (h - 1) )
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

	threading::get().dispatch( std::bind( pdtncc_gradAve_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( gxb ), std::cref( ab ), std::cref( bb ) ), 0, bx.height() );
}

////////////////////////////////////////

static void
pdtncc_dualUpdate_thread( size_t, int s, int e, plane_buffer &p1, plane_buffer &p2, plane_buffer &p3, plane_buffer &p4, plane_buffer &p5, plane_buffer &p6, const const_plane_buffer &u_, const const_plane_buffer &v_, const const_plane_buffer &w_, float sigma )
{
	int w = p1.width();
	int h = p1.height();
	int wm1 = w - 1;
	int hm1 = h - 1;
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
		if ( y < hm1 )
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

			float reproj = 1.F / std::max( 1.F, std::sqrt( p1v * p1v + p2v * p2v + p3v * p3v + p4v * p4v ) );
			p1L[x] = p1v * reproj;
			p2L[x] = p2v * reproj;
			p3L[x] = p3v * reproj;
			p4L[x] = p4v * reproj;

			reproj = 1.F / std::max( 1.F, std::sqrt( p5v * p5v + p6v * p6v ) );
			p5L[x] = p5v * reproj;
			p6L[x] = p6v * reproj;
		}
	}
}

static void
pdtncc_dualUpdate( plane &p0, plane &p1, plane &p2, plane &p3, plane &p4, plane &p5, const plane &u, const plane &v, const plane &w, float sigma )
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

	threading::get().dispatch( std::bind( pdtncc_dualUpdate_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( p0b ), std::ref( p1b ), std::ref( p2b ), std::ref( p3b ), std::ref( p4b ), std::ref( p5b ), std::cref( ub ), std::cref( vb ), std::cref( wb ), sigma ), 0, u.height() );
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

		if ( y > 0 )
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

			float gradSqr = std::max( eps, dXV * dXV + dYV * dYV + gamv * gamv );
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
pdtncc_primalRho2( plane &u_, plane &u, plane &v_, plane &v, plane &w_, plane &w, const plane &p0, const plane &p1, const plane &p2, const plane &p3, const plane &p4, const plane &p5, const plane &dT, const plane &u0, const plane &v0, const plane &dX, const plane &dY, float tau, float lambda, float gamv )
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

	threading::get().dispatch( std::bind( pdtncc_primalRho2_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref(u_b), std::ref(ub), std::ref(v_b), std::ref(vb), std::ref(w_b), std::ref(wb), std::cref( p0b ), std::cref( p1b ), std::cref( p2b ), std::cref( p3b ), std::cref( p4b ), std::cref( p5b ), std::cref( dTb ), std::cref( u0b ), std::cref( v0b ), std::cref( dXb ), std::cref( dYb ), tau, lambda, gamv ), 0, u.height() );
}

////////////////////////////////////////

static vector_field
runPD( const plane &a, const plane &b, float lambda, float theta, float gamv, int innerIters, int warpIters, float eta )
{
	TODO( "Add ability to pass in initial u, v" );
	static bool first = true;
	if ( first )
	{
		std::cout << "switch resize to bspline when ready" << std::endl;
		first = false;
	}

	std::vector<plane> hierA = make_pyramid( a, "bicubic", eta, 0, 16 );
	std::vector<plane> hierB = make_pyramid( b, "bicubic", eta, 0, 16 );

	float tau = 1.0 / ::sqrt( 8.0 );
	float sigma = tau; // 1.F / L

	plane u, v;
	plane lumW;
	plane p[6];
	while ( ! hierA.empty() )
	{
		plane curA = hierA.back();
		plane curB = hierB.back();
		int curW = curA.width();
		int curH = curA.height();
		hierA.pop_back();
		hierB.pop_back();

		if ( ! u.valid() )
		{
			u = create_plane( curW, curH, 0.F );
			v = u.copy();
			lumW = u.copy();
			for ( int i = 0; i != 6; ++i )
				p[i] = u.copy();
		}
		else
		{
			float scaleX = static_cast<float>( curW ) / static_cast<float>( u.width() );
			float scaleY = static_cast<float>( curH ) / static_cast<float>( u.height() );

			u = resize_bicubic( u, curW, curH ) * scaleX;
			v = resize_bicubic( v, curW, curH ) * scaleY;

			lumW = resize_bicubic( lumW, curW, curH );
			for ( int i = 0; i != 6; ++i )
				p[i] = resize_bicubic( p[i], curW, curH );
		}

		plane u_ = u.copy();
		plane v_ = v.copy();
		plane lumw_ = lumW.copy();

		plane dX, dY;
		plane aX, aY;

		bool splitGrads = true;
		if ( hierA.empty() )
		{
			dX = noise_gradient_horiz5( curA );
			dirichlet( dX );
			dY = noise_gradient_vert5( curA );
			dirichlet( dY );
			splitGrads = false;
		}
		else
		{
			dX = plane( curW, curH );
			dY = plane( curW, curH );
			aX = noise_gradient_horiz5( curA );
			aY = noise_gradient_vert5( curA );
		}

		int wI = std::max( 1, warpIters - static_cast<int>( hierA.size() ) );
		int iI = std::max( 1, innerIters / wI );

		for ( int w = 0; w != wI; ++w )
		{
			plane u0 = u.copy();
			plane v0 = v.copy();

			plane dT = warp_bilinear( curB, vector_field::create( u, v, false ) );
			if ( splitGrads )
			{
				plane bX, bY;
				if ( curW <= 512 )
				{
					bX = central_gradient_horiz( dT );
					bY = central_gradient_vert( dT );
				}
				else
				{
					bX = noise_gradient_horiz5( dT );
					bY = noise_gradient_vert5( dT );
				}
//				plane bX = noise_gradient_horiz5( dT );
				pdtncc_gradAve( dX, aX, bX );
//				plane bY = noise_gradient_vert5( dT );
				pdtncc_gradAve( dY, aY, bY );
			}

			dT = dT - curA;
			dirichlet( dT );

			for ( int i = 0; i != iI; ++i )
			{
				// update the dual variables
				pdtncc_dualUpdate( p[0], p[1], p[2], p[3], p[4], p[5], u_, v_, lumw_, sigma );

				// update the primal
				pdtncc_primalRho2( u_, u, v_, v, lumw_, lumW, p[0], p[1], p[2], p[3], p[4], p[5], dT, u0, v0, dX, dY, tau, lambda, gamv );
			}
		}

		removePeaks( u, v, 3 );
	}
	
	return vector_field::create( std::move( u ), std::move( v ), false );
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

vector_field
oflow_ahtvl1( const plane &a, const plane &b, float lambda, float theta, float epsilon, float edgePower, float edgeAlpha, int edgeBorder, int tvl1Iters, int warpIters, float eta )
{
	engine::dimensions d;
	precondition( a.dims() == b.dims(), "oflow_ahtvl1 must have a & b of same size, received {0}x{1} and {3}x{4}", a.width(), a.height(), b.width(), b.height() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;

	return vector_field( true, "p.oflow_ahtvl1", d, a, b, lambda, theta, epsilon, edgePower, edgeAlpha, edgeBorder, tvl1Iters, warpIters, eta );
}

////////////////////////////////////////

vector_field
oflow_primaldual( const plane &a, const plane &b, float lambda, float theta, float sigma, int innerIters, int warpIters, float eta )
{
	engine::dimensions d;
	precondition( a.dims() == b.dims(), "oflow_htvl1 must have a & b of same size, received {0}x{1} and {3}x{4}", a.width(), a.height(), b.width(), b.height() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;

	return vector_field( true, "p.oflow_primaldual", d, a, b, lambda, theta, sigma, innerIters, warpIters, eta );
}

////////////////////////////////////////

void
add_oflow( engine::registry &r )
{
	using namespace engine;

	r.register_constant<image::vector_field>();
	r.add( op( "v.extract_u", extract_u, op::simple ) );
	r.add( op( "v.extract_v", extract_v, op::simple ) );

	r.add( op( "p.oflow_ahtvl1", runAHTVL1, op::threaded ) );
	r.add( op( "p.oflow_primaldual", runPD, op::threaded ) );

	add_vector_ops( r );
	add_patchmatch( r );
}

////////////////////////////////////////

} // namespace image

