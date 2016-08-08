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

#include "optical_flow.h"
#include "plane_ops.h"
#include "threading.h"

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
			float norm = sqrtf( gxv * gxv + gyv * gyv );
			ewLine[x] = fmaxf( eps, expf( eWeight * powf( norm, ePower ) ) );
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

	return ba;
}

////////////////////////////////////////

static void
peakThread( size_t, int s, int e, plane_buffer &u, const plane_buffer &mu )
{
	int w = u.width();

	for ( int hy = s; hy < e; ++hy )
	{
		float *uLine = u.line( hy );
		const float *muLine = mu.line( hy );
		for ( int hx = 0; hx < w; ++hx )
		{
			float uV = uLine[hx];
			float muV = muLine[hx];
			float sumMU = 0.F;
			for ( int y = -2; y <= 2; ++y )
			{
				for ( int x = -2; x <= 2; ++x )
				{
					sumMU += get_hold( mu, hx + x, hy + y );
				}
			}
			float aveMU = sumMU / 25.F;
			if ( fabsf( uV - muV ) > aveMU )
				uV = muV;
		}
	}
}

static void
removePeaks( plane &u, plane &v )
{
	{
		plane mu = median( u, 3 );
		plane_buffer mub = mu;
		plane_buffer ub = u;
		threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ub ), std::cref( mub ) ), 0, u.height() );
	}

	plane mv = median( v, 3 );
	plane_buffer mvb = mv;
	plane_buffer vb = v;
	threading::get().dispatch( std::bind( peakThread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( vb ), std::cref( mvb ) ), 0, v.height() );
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
			float magSq = fmaxf( eps, gXAve * gXAve + gYAve * gYAve );

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

				float scale = fmaxf( 1.F, sqrtf( outU * outU + outV * outV ) );
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

				float scale = fmaxf( 1.F, sqrtf( outU * outU + outV * outV ) );
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

				float scale = fmaxf( 1.F, sqrtf( outU * outU + outV * outV ) / eW );
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
				float eW = eLine[x];
				float uX = uLine[x];
				float uY = 0.F;
				if ( x < (w - 1) )
					uX = uLine[x + 1] - uX;

				outU = outU + tau * ( uX - epsilon * outU );
				outV = outV + tau * ( uY - epsilon * outV );

				float scale = fmaxf( 1.F, sqrtf( outU * outU + outV * outV ) / eW );
				outU = outU / scale;
				outV = outV / scale;
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
	std::vector<float> gradKern
	{
		1.F / 12.F,
		-8.F / 12.F,
		0.F,
		8.F / 12.F,
		-1.F / 12.F
	};
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
		{
			edgeW = ahtvl1_edgeWeight( curA, edgeKern, edgePower, edgeAlpha, edgeBorder );
		}
		plane ax = convolve_horiz( curA, gradKern );
		plane ay = convolve_vert( curA, gradKern );
		plane bx = convolve_horiz( curB, gradKern );
		plane by = convolve_vert( curB, gradKern );

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

		removePeaks( u, v );
	}

	return vector_field::create( std::move( u ), std::move( v ), false );
}

////////////////////////////////////////

vector_field
runPD( const plane &a, const plane &b, float lambda, float theta, float gamma, int innerIters, int warpIters, float eta )
{
	plane u = create_plane( a.width(), a.height(), 0.F );
	plane v = create_plane( a.width(), a.height(), 0.F );

	std::vector<plane> hierA = make_pyramid( a, "bicubic", eta, 0, 16 );
	std::vector<plane> hierB = make_pyramid( b, "bicubic", eta, 0, 16 );

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
oflow_primaldual( const plane &a, const plane &b, float lambda, float theta, float gamma, int innerIters, int warpIters, float eta )
{
	engine::dimensions d;
	precondition( a.dims() == b.dims(), "oflow_htvl1 must have a & b of same size, received {0}x{1} and {3}x{4}", a.width(), a.height(), b.width(), b.height() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;

	return vector_field( true, "p.oflow_primaldual", d, a, b, lambda, theta, gamma, innerIters, warpIters, eta );
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
	r.add( op( "p.oflow_primaludal", runPD, op::threaded ) );

	add_vector_ops( r );
	add_patchmatch( r );
}

////////////////////////////////////////

} // namespace image

