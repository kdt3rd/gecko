//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
horiz_cgrad( scanline &dest, const scanline &src )
{
	if ( dest.width() < 2 )
	{
		dest[0] = 0.F;
		return;
	}
	precondition( dest.get() != src.get(), "Need not-in-place flag to op" );

	dest[0] = ( src[1] - src[0] ) / 2.F;
	int wm1 = dest.width() - 1;
	for ( int x = 1; x < wm1; ++x )
		dest[x] = ( src[x+1] - src[x-1] ) / 2.F;
	dest[wm1] = ( src[wm1] - src[wm1-1] ) / 2.F;
}

////////////////////////////////////////

static void
vert_cgrad( scanline &dest, int y, const plane &src )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	int sy1 = std::min( int(src.height() - 1), y + 1 );
	int sym1 = std::max( int(0), y - 1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline m1 = scan_ref( src, sym1 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = ( p1[x] - m1[x] ) / 2.F;
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
	dest[0] = ( ( src[1] - src[0] ) * 8.F + ( src[0] - src[2] ) ) / 12.F;
	dest[1] = ( ( src[2] - src[0] ) * 8.F + ( src[0] - src[3] ) ) / 12.F;
	for ( int x = 2; x < wm2; ++x )
		dest[x] = ( ( src[x+1] - src[x-1] ) * 8.F + ( src[x-2] - src[x+2] ) ) / 12.F;
	dest[wm2] = ( ( src[wm1] - src[wm2-1] ) * 8.F + ( src[wm2-2] - src[wm1] ) ) / 12.F;
	dest[wm1] = ( ( src[wm1] - src[wm2] ) * 8.F + ( src[wm2-1] - src[wm1] ) ) / 12.F;
}

////////////////////////////////////////

static void
vert_ngrad5( scanline &dest, int y, const plane &src )
{
	precondition( dest.get() != src.line( y ), "Need not-in-place flag to op" );

	int sy1 = std::min( int(src.height() - 1), y + 1 );
	int sy2 = std::min( int(src.height() - 1), y + 2 );
	int sym1 = std::max( int(0), y - 1 );
	int sym2 = std::max( int(0), y - 2 );
	scanline m2 = scan_ref( src, sym2 );
	scanline m1 = scan_ref( src, sym1 );
	scanline p1 = scan_ref( src, sy1 );
	scanline p2 = scan_ref( src, sy2 );
	for ( int x = 0, w = dest.width(); x < w; ++x )
		dest[x] = ( ( p1[x] - m1[x] ) * 8.F + ( m2[x] - p2[x] ) ) / 12.F;
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

	int sy1 = std::min( int(src.height() - 1), y + 1 );
	int sym1 = std::max( int(0), y - 1 );
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

	int sy1 = std::min( int(src.height() - 1), y + 1 );
	int sym1 = std::max( int(0), y - 1 );
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
	int hm1 = src.height() - 1;

	for ( int l = - halfK; l <= halfK; ++l )
	{
		float kVal = k[static_cast<size_t>( l + halfK )];
		int curY = std::max( int(0), std::min( hm1, y + l ) );
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

	r.add( op( "p.cgrad_h", base::choose_runtime( horiz_cgrad ), scanline_plane_adapter<false, decltype(horiz_cgrad)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.cgrad_v", base::choose_runtime( vert_cgrad ), n_scanline_plane_adapter<false, decltype(vert_cgrad)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.ngrad_h5", base::choose_runtime( horiz_ngrad5 ), scanline_plane_adapter<false, decltype(horiz_ngrad5)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.ngrad_v5", base::choose_runtime( vert_ngrad5 ), n_scanline_plane_adapter<false, decltype(vert_ngrad5)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.sep_conv3_mirror_h", base::choose_runtime( horiz_convolve3_mirror ), scanline_plane_adapter<false, decltype(horiz_convolve3_mirror)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv3_h", base::choose_runtime( horiz_convolve3 ), scanline_plane_adapter<false, decltype(horiz_convolve3)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv_h", base::choose_runtime( horiz_convolve ), scanline_plane_adapter<false, decltype(horiz_convolve)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.sep_conv3_mirror_v", base::choose_runtime( vert_convolve3_mirror ), n_scanline_plane_adapter<false, decltype(vert_convolve3_mirror)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv3_v", base::choose_runtime( vert_convolve3 ), n_scanline_plane_adapter<false, decltype(vert_convolve3)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv_v", base::choose_runtime( vert_convolve ), n_scanline_plane_adapter<false, decltype(vert_convolve)>(), dispatch_scan_processing, op::n_to_one ) );
}

////////////////////////////////////////
} // image



