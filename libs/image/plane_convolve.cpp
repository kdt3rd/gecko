// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "plane_convolve.h"
#include <base/contract.h>
#include <base/math_functions.h>
#include <base/cpu_features.h>
#include "scanline_process.h"
#include "threading.h"

////////////////////////////////////////

namespace
{

using namespace image;

////////////////////////////////////////

static void
horiz_igrad( scanline &dest, const scanline &src )
{
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	dest[0] = 0.F;
	int w = dest.width();
	for ( int x = 1; x < w; ++x )
		dest[x] = ( src[x] - src[x-1] );
}

////////////////////////////////////////

static void
vert_igrad( scanline &dest, int y, const plane &src )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	if ( y == src.y1() )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
	}
	else
	{
		scanline cur = scan_ref( src, y );
		scanline m1 = scan_ref( src, y - 1 );
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = ( cur[x] - m1[x] );
	}
}

////////////////////////////////////////

static void
horiz_cgrad( scanline &dest, const scanline &src )
{
	if ( dest.width() < 2 )
	{
		dest[0] = 0.F;
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

//	dest[0] = ( src[1] - src[0] );
	dest[0] = 0.F;
	int wm1 = dest.width() - 1;
	for ( int x = 1; x < wm1; ++x )
		dest[x] = ( src[x+1] - src[x-1] ) * 0.5F;
//	dest[wm1] = ( src[wm1] - src[wm1-1] );
	dest[wm1] = 0.F;
}

////////////////////////////////////////

static void
vert_cgrad( scanline &dest, int y, const plane &src )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	if ( y == src.y1() || y == src.y2() )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
		return;
	}
	int sy1 = std::min( src.y2(), y + 1 );
	int sym1 = std::max( src.y1(), y - 1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline m1 = scan_ref( src, sym1 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = ( p1[x] - m1[x] ) * 0.5F;
}

static void
horiz_ngrad5( scanline &dest, const scanline &src )
{
	if ( dest.width() < 5 )
	{
		horiz_cgrad( dest, src );
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	int wm1 = dest.width() - 1;
	int wm2 = dest.width() - 2;
	dest[0] = 0.F;//( ( src[1] - src[0] ) * 8.F + ( src[0] - src[2] ) ) / 12.F;
	dest[1] = 0.F;//( ( src[2] - src[0] ) * 8.F + ( src[0] - src[3] ) ) / 12.F;

	for ( int x = 2; x < wm2; ++x )
		dest[x] = ( ( src[x+1] - src[x-1] ) * 8.F + ( src[x-2] - src[x+2] ) ) / 12.F;

	dest[wm2] = 0.F;//( ( src[wm1] - src[wm2-1] ) * 8.F + ( src[wm2-2] - src[wm1] ) ) / 12.F;
	dest[wm1] = 0.F;//( ( src[wm1] - src[wm2] ) * 8.F + ( src[wm2-1] - src[wm1] ) ) / 12.F;
}

////////////////////////////////////////

static void
vert_ngrad5( scanline &dest, int y, const plane &src )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	if ( y <= ( src.y1() + 1 ) || y >= ( src.y2() - 1 ) )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
		return;
	}

	int sy1 = std::min( src.y2(), y + 1 );
	int sy2 = std::min( src.y2(), y + 2 );
	int sym1 = std::max( src.y1(), y - 1 );
	int sym2 = std::max( src.y1(), y - 2 );
	scanline m2 = scan_ref( src, sym2 );
	scanline m1 = scan_ref( src, sym1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline p2 = scan_ref( src, sy2 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = ( ( p1[x] - m1[x] ) * 8.F + ( m2[x] - p2[x] ) ) / 12.F;
}

////////////////////////////////////////

static void
horiz_igrad_alpha( scanline &dest, const scanline &src, const scanline &alpha )
{
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	dest[0] = 0.F;
	int w = dest.width();
	float curAlpha = alpha[0];
	for ( int x = 1; x < w; ++x )
	{
		float nextAlpha = alpha[x];
		float out = 0.F;
		if ( curAlpha > 0.F && nextAlpha > 0.F )
			out = src[x] - src[x-1];
		dest[x] = out;
		curAlpha = nextAlpha;
	}
}

////////////////////////////////////////

static void
vert_igrad_alpha( scanline &dest, int y, const plane &src, const plane &alpha )
{
	precondition( dest.get() != src.line( y ) && dest.get() != alpha.line( y ), "Need not-in-place flag to op" );

	if ( y == src.y1() )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
	}
	else
	{
		scanline cur = scan_ref( src, y );
		scanline aL = scan_ref( alpha, y );
		scanline m1 = scan_ref( src, y - 1 );
		scanline m1AL = scan_ref( alpha, y - 1 );
		for ( int x = 0, w = dest.width(); x < w; ++x )
		{
			float out = 0.F;
			if ( aL[x] > 0.F && m1AL[x] > 0.F )
				out = cur[x] - m1[x];
			dest[x] = out;
		}
	}
}

////////////////////////////////////////

static void
horiz_cgrad_alpha( scanline &dest, const scanline &src, const scanline &alpha )
{
	if ( dest.width() < 2 )
	{
		dest[0] = 0.F;
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	float prevA = alpha[0];
	float curA = prevA;
	float nextA = alpha[1];
	float curV = src[0];
	float prevV = curV;
	float nextV = src[1];
//	if ( prevA > 0.F && nextA > 0.F )
//		dest[0] = ( nextV - prevV );
//	else
		dest[0] = 0.F;
	int wm1 = dest.width() - 1;
	for ( int x = 1; x < wm1; ++x )
	{
		prevA = curA;
		curA = nextA;
		nextA = alpha[x+1];

		prevV = curV;
		curV = nextV;
		nextV = src[x+1];

		float grad = 0.F;
		if ( curA > 0.F )
		{
			if ( nextA > 0.F && prevA > 0.F )
				grad = ( nextV - prevV ) * 0.5F;
//			else if ( nextA > 0.F )
//				grad = ( nextV - curV );
			else if ( prevA > 0.F )
				grad = ( curV - prevV );
		}
		dest[x] = grad;
	}
//	if ( curA > 0.F && prevA > 0.F )
//		dest[wm1] = ( src[wm1] - src[wm1-1] );
//	else
		dest[wm1] = 0.F;
}

////////////////////////////////////////

static void
vert_cgrad_alpha( scanline &dest, int y, const plane &src, const plane &alpha )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );
	if ( src.height() == 1 )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
	}

	if ( y == src.y1() || y == src.y2() )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
		return;
	}

//	if ( y == src.y1() )
//	{
//		for ( int x = 0, w = dest.width(); x < w; ++x )
//			dest[x] = 0.F;
//	}
//	else if ( y == src.y2() )
//	{
//		vert_igrad_alpha( dest, y, src, alpha );
//	}
//	else
	{
		scanline p1 = scan_ref( src, y + 1 );
		scanline m1 = scan_ref( src, y - 1 );
		scanline cur = scan_ref( src, y );
		scanline ap1 = scan_ref( alpha, y + 1 );
		scanline am1 = scan_ref( alpha, y - 1 );
		scanline ac = scan_ref( alpha, y );
		for ( int x = 0, w = dest.width(); x < w; ++x )
		{
			float grad = 0.F;
			if ( ac[x] > 0.F )
			{
				float nextA = ap1[x];
				float prevA = am1[x];
				if ( nextA > 0.F && prevA > 0.F )
					grad = ( p1[x] - m1[x] ) * 0.5F;
//				else if ( nextA > 0.F )
//					grad = ( p1[x] - cur[x] );
				else if ( prevA > 0.F )
					grad = ( cur[x] - m1[x] );
			}
			dest[x] = grad;
		}
	}
}

static void
horiz_ngrad5_alpha( scanline &dest, const scanline &src, const scanline &alpha )
{
	if ( dest.width() < 5 )
	{
		horiz_cgrad_alpha( dest, src, alpha );
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );
	int wm1 = dest.width() - 1;
	int wm2 = dest.width() - 2;

	float ppA = alpha[0];
	float pA = ppA;
	float cA = pA;
	float nA = alpha[1];
	float nnA = alpha[2];

	float cV = src[0];
	float pV = cV;
	float ppV = cV;
	float nV = src[1];
	float nnV = src[2];
	float grad = 0.F;
//	if ( pA > 0.F )
//	{
//		if ( nA > 0.F && nnA > 0.F )
//			grad = ( ( nV - pV ) * 2.F + ( nnV - ppV ) ) * 0.125F;
//		else if ( nA > 0.F )
//			grad = ( nV - pV );
//		else if ( nnA > 0.F )
//			grad = ( nnV - ppV ) / 2.F;
//	}
	dest[0] = 0.F;
	ppA = pA; pA = cA; cA = nA; nA = nnA; nnA = alpha[3];
	ppV = pV; pV = cV; cV = nV; nV = nnV; nnV = src[3];
	grad = 0.F;
	if ( cA > 0.F )
	{
		if ( pA > 0.F && nA > 0.F )
			grad = ( nV - pV ) * 0.5F;
//		else if ( nA > 0.F )
//			grad = ( nV - cV );
		else if ( pA > 0.F )
			grad = ( cV - pV );
	}
//	dest[1] = grad;
	dest[1] = 0.F;
	for ( int x = 2; x < wm2; ++x )
	{
		ppA = pA; pA = cA; cA = nA; nA = nnA; nnA = alpha[x+2];
		ppV = pV; pV = cV; cV = nV; nV = nnV; nnV = src[x+2];
		grad = 0.F;
		if ( cA > 0.F )
		{
			if ( pA > 0.F && nA > 0.F && ppA > 0.F && nnA > 0.F )
				grad = ( ( nV - pV ) * 2.F + ( nnV - ppV ) ) * 0.125F;
			else if ( pA > 0.F && nA > 0.F )
				grad = ( nV - pV ) * 0.5F;
//			else if ( nA > 0.F )
//				grad = ( nV - cV );
			else if ( pA > 0.F )
				grad = ( cV - pV );
		}
		dest[x] = grad;
	}
	ppA = pA; pA = cA; nA = nnA;
	ppV = pV; pV = cV; nV = nnV;
	grad = 0.F;
	if ( cA > 0.F )
	{
		if ( pA > 0.F && nA > 0.F )
			grad = ( nV - pV ) * 0.5F;
//		else if ( nA > 0.F )
//			grad = ( nV - cV );
		else if ( pA > 0.F )
			grad = ( cV - pV );
	}
//	dest[wm2] = grad;
	dest[wm2] = 0.F;
	ppA = pA; pA = cA;
	ppV = pV; pV = cV;
	grad = 0.F;
	if ( cA > 0.F && pA > 0.F)
		grad = ( cV - pV );
//	dest[wm1] = grad;
	dest[wm1] = 0.F;
}

////////////////////////////////////////

static void
vert_ngrad5_alpha( scanline &dest, int y, const plane &src, const plane &alpha )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	if ( y <= ( src.y1() + 1 ) || y >= ( src.y2() - 1 ) )
	{
		for ( int x = 0, w = dest.width(); x < w; ++x )
			dest[x] = 0.F;
		return;
	}

//	if ( y >= (src.y1() + 2) && y <= (src.y2() - 2) )
	{
		scanline m2 = scan_ref( src, y - 2 );
		scanline m1 = scan_ref( src, y - 1 );
		scanline cur = scan_ref( src, y );
		scanline p1 = scan_ref( src, y + 1 );
		scanline p2 = scan_ref( src, y + 2 );
		scanline am2 = scan_ref( alpha, y - 2 );
		scanline am1 = scan_ref( alpha, y - 1 );
		scanline acur = scan_ref( alpha, y );
		scanline ap1 = scan_ref( alpha, y + 1 );
		scanline ap2 = scan_ref( alpha, y + 2 );

		int w = src.width();
		for ( int x = 0; x < w; ++x )
		{
			float ppA = am2[x], pA = am1[x], cA = acur[x], nA = ap1[x], nnA = ap2[x];
			float ppV = m2[x], pV = m1[x], cV = cur[x], nV = p1[x], nnV = p2[x];
			float grad = 0.F;
			if ( cA > 0.F )
			{
				if ( pA > 0.F && nA > 0.F && ppA > 0.F && nnA > 0.F )
					grad = ( ( nV - pV ) * 2.F + ( nnV - ppV ) ) * 0.125F;
				else if ( pA > 0.F && nA > 0.F )
					grad = ( nV - pV ) * 0.5F;
//				else if ( nA > 0.F )
//					grad = ( nV - cV );
				else if ( pA > 0.F )
					grad = ( cV - pV );
			}
			dest[x] = grad;
		}
	}
//	else
//		vert_cgrad_alpha( dest, y, src, alpha );
}

static void
horiz_convolve3_mirror( scanline &dest, const scanline &src, float outer, float center )
{
	if ( dest.width() < 2 )
	{
		dest[0] = src[0];
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	dest[0] = ( src[0] + src[1] ) * outer + src[0] * center;
	int wm1 = dest.width() - 1;
	for ( int x = 1; x < wm1; ++x )
		dest[x] = ( src[x+1] + src[x-1] ) * outer + src[x] * center;
	dest[wm1] = ( src[wm1] + src[wm1-1] ) * outer + src[wm1] * center;
}

static void
horiz_convolve3( scanline &dest, const scanline &src, float left, float center, float right )
{
	if ( dest.width() < 2 )
	{
		dest[0] = src[0];
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	dest[0] = src[0] * ( left + center ) + src[1] * right;
	int wm1 = dest.width() - 1;
	for ( int x = 1; x < wm1; ++x )
		dest[x] = src[x-1] * left + src[x] * center + src[x+1] * right;
	dest[wm1] = ( src[wm1] * ( center + right ) + src[wm1-1] ) * left;
}

static void
horiz_convolve( scanline &dest, const scanline &src, const std::vector<float> &k )
{
	if ( dest.width() < 2 )
	{
		dest[0] = src[0];
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	int halfK = static_cast<int>( k.size() / 2 );
	int wm1 = dest.width() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		float sum = 0.F;
		for ( int l = - halfK; l <= halfK; ++l )
		{
			int pos = std::max( 0, std::min( wm1, x + l ) );
			sum += src[pos] * k[static_cast<size_t>( l + halfK )];
		}
		dest[x] = sum;
	}
}

////////////////////////////////////////

static void
vert_convolve3_mirror( scanline &dest, int y, const plane &src, float outer, float center )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	int sy1 = std::min( src.y2(), y + 1 );
	int sym1 = std::max( src.y1(), y - 1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline c1 = scan_ref( src, y );
	scanline m1 = scan_ref( src, sym1 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = ( p1[x] + m1[x] ) * outer + c1[x] * center;
}

////////////////////////////////////////

static void
vert_convolve3( scanline &dest, int y, const plane &src, float left, float center, float right )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	int sy1 = std::min( src.y2(), y + 1 );
	int sym1 = std::max( src.y1(), y - 1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline c1 = scan_ref( src, y );
	scanline m1 = scan_ref( src, sym1 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = m1[x] * left + c1[x] * center + p1[x] * right;
}

////////////////////////////////////////

static void
vert_convolve( scanline &dest, int y, const plane &src, const std::vector<float> &k )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	int halfK = static_cast<int>( k.size() / 2 );

	for ( int l = - halfK; l <= halfK; ++l )
	{
		float kVal = k[static_cast<size_t>( l + halfK )];
		int curY = std::max( src.y1(), std::min( src.y2(), y + l ) );
		scanline s = scan_ref( src, curY );
		if ( l == - halfK )
		{
			for ( int x = 0, w = dest.width(); x < w; ++x )
				dest[x] = s[x] * kVal;
		}
		else
		{
			for ( int x = 0, w = dest.width(); x < w; ++x )
				dest[x] += s[x] * kVal;
		}
	}
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane instant_gradient_horiz( const plane &p )
{
	return plane( "p.igrad_h", p.dims(), p );
}

////////////////////////////////////////

plane instant_gradient_vert( const plane &p )
{
	return plane( "p.igrad_v", p.dims(), p );
}

////////////////////////////////////////

plane central_gradient_horiz( const plane &p )
{
	return plane( "p.cgrad_h", p.dims(), p );
}

////////////////////////////////////////

plane central_gradient_vert( const plane &p )
{
	return plane( "p.cgrad_v", p.dims(), p );
}

////////////////////////////////////////

plane noise_gradient_horiz5( const plane &p )
{
	return plane( "p.ngrad_h5", p.dims(), p );
}

////////////////////////////////////////

plane noise_gradient_vert5( const plane &p )
{
	return plane( "p.ngrad_v5", p.dims(), p );
}

////////////////////////////////////////

plane instant_gradient_horiz( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.igrad_h_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane instant_gradient_vert( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.igrad_v_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane central_gradient_horiz( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.cgrad_h_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane central_gradient_vert( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.cgrad_v_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane noise_gradient_horiz5( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.ngrad_h5_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane noise_gradient_vert5( const plane &p, const plane &alpha )
{
	precondition( p.dims() == alpha.dims(), "alpha based gradient requested with different sized planes {0} vs alpha {1}", p.dims(), alpha.dims() );
	return plane( "p.ngrad_v5_alpha", p.dims(), p, alpha );
}

////////////////////////////////////////

plane convolve_horiz( const plane &p, const std::vector<float> &k )
{
	if ( k.size() == 3 )
	{
		if ( base::equal( k[0], k[2] ) )
			return plane( "p.sep_conv3_mirror_h", p.dims(), p, k[0], k[1] );

		return plane( "p.sep_conv3_h", p.dims(), p, k[0], k[1], k[2] );
	}

	precondition( k.size() % 2 != 0, "non-odd-sized kernel {0}", k.size() );
	return plane( "p.sep_conv_h", p.dims(), p, k );
}

////////////////////////////////////////

plane convolve_vert( const plane &p, const std::vector<float> &k )
{
	if ( k.size() == 3 )
	{
		if ( base::equal( k[0], k[2] ) )
			return plane( "p.sep_conv3_mirror_v", p.dims(), p, k[0], k[1] );

		return plane( "p.sep_conv3_v", p.dims(), p, k[0], k[1], k[2] );
	}

	precondition( k.size() % 2 != 0, "non-odd-sized kernel {0}", k.size() );
	return plane( "p.sep_conv_v", p.dims(), p, k );
}

////////////////////////////////////////

void add_convolve( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.igrad_h", base::choose_runtime( horiz_igrad ), scanline_plane_adapter<false, decltype(horiz_igrad)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.igrad_v", base::choose_runtime( vert_igrad ), n_scanline_plane_adapter<false, decltype(vert_igrad)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.cgrad_h", base::choose_runtime( horiz_cgrad ), scanline_plane_adapter<false, decltype(horiz_cgrad)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.cgrad_v", base::choose_runtime( vert_cgrad ), n_scanline_plane_adapter<false, decltype(vert_cgrad)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.ngrad_h5", base::choose_runtime( horiz_ngrad5 ), scanline_plane_adapter<false, decltype(horiz_ngrad5)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.ngrad_v5", base::choose_runtime( vert_ngrad5 ), n_scanline_plane_adapter<false, decltype(vert_ngrad5)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.igrad_h_alpha", base::choose_runtime( horiz_igrad_alpha ), scanline_plane_adapter<false, decltype(horiz_igrad_alpha)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.igrad_v_alpha", base::choose_runtime( vert_igrad_alpha ), n_scanline_plane_adapter<false, decltype(vert_igrad_alpha)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.cgrad_h_alpha", base::choose_runtime( horiz_cgrad_alpha ), scanline_plane_adapter<false, decltype(horiz_cgrad_alpha)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.cgrad_v_alpha", base::choose_runtime( vert_cgrad_alpha ), n_scanline_plane_adapter<false, decltype(vert_cgrad_alpha)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.ngrad_h5_alpha", base::choose_runtime( horiz_ngrad5_alpha ), scanline_plane_adapter<false, decltype(horiz_ngrad5_alpha)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.ngrad_v5_alpha", base::choose_runtime( vert_ngrad5_alpha ), n_scanline_plane_adapter<false, decltype(vert_ngrad5_alpha)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.sep_conv3_mirror_h", base::choose_runtime( horiz_convolve3_mirror ), scanline_plane_adapter<false, decltype(horiz_convolve3_mirror)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv3_h", base::choose_runtime( horiz_convolve3 ), scanline_plane_adapter<false, decltype(horiz_convolve3)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv_h", base::choose_runtime( horiz_convolve ), scanline_plane_adapter<false, decltype(horiz_convolve)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.sep_conv3_mirror_v", base::choose_runtime( vert_convolve3_mirror ), n_scanline_plane_adapter<false, decltype(vert_convolve3_mirror)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv3_v", base::choose_runtime( vert_convolve3 ), n_scanline_plane_adapter<false, decltype(vert_convolve3)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv_v", base::choose_runtime( vert_convolve ), n_scanline_plane_adapter<false, decltype(vert_convolve)>(), dispatch_scan_processing, op::n_to_one ) );
}

////////////////////////////////////////
} // image



