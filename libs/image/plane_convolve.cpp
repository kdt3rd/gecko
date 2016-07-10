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

	r.add( op( "p.sep_conv3_mirror_h", base::choose_runtime( horiz_convolve3_mirror ), scanline_plane_adapter<false, decltype(horiz_convolve3_mirror)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv3_h", base::choose_runtime( horiz_convolve3 ), scanline_plane_adapter<false, decltype(horiz_convolve3)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "p.sep_conv_h", base::choose_runtime( horiz_convolve ), scanline_plane_adapter<false, decltype(horiz_convolve)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.sep_conv3_mirror_v", base::choose_runtime( vert_convolve3_mirror ), n_scanline_plane_adapter<false, decltype(vert_convolve3_mirror)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv3_v", base::choose_runtime( vert_convolve3 ), n_scanline_plane_adapter<false, decltype(vert_convolve3)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.sep_conv_v", base::choose_runtime( vert_convolve ), n_scanline_plane_adapter<false, decltype(vert_convolve)>(), dispatch_scan_processing, op::n_to_one ) );
}

////////////////////////////////////////
} // image



