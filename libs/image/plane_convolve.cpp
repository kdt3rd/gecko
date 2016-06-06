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
	int n = - halfK + static_cast<int>( k.size() );
	int w = dest.width();
	for ( int x = 0; x < w; ++x )
	{
		float sum = 0.F;
		for ( int l = - halfK; l <= n; ++l )
		{
			int pos = std::max( 0, std::min( w - 1, x + l ) );
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
	int n = - halfK + static_cast<int>( k.size() );
	int hm1 = src.height() - 1;

	for ( int l = - halfK; l <= n; ++l )
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

////////////////////////////////////////

inline void sort( float &a, float &b )
{
	float t = fmaxf( a, b );
	a = fminf( a, b );
	b = t;
}
#define mn3(a,b,c) sort(a,b); sort(a,c);
#define mx3(a,b,c) sort(b,c); sort(a,c);
#define mnmx3(a,b,c) mx3(a,b,c); sort(a,b);
#define mnmx4(a,b,c,d) sort(a,b); sort(c,d); sort(a,c); sort(b,d);
#define mnmx5(a,b,c,d,e) sort(a,b); sort(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) sort(a,d); sort(b,e); sort(c,f); mn3(a,b,c); mx3(d,e,f);
#define mnmx9(a,b,c,d,e,f,g,h,k) sort(a,d); sort(b,e); sort(c,f); mn3(a,b,c); mx3(d,e,f); mnmx5(g,b,c,d,e); mnmx4(h,b,c,d); mnmx3(k,b,c);

////////////////////////////////////////

void
median_3x3( scanline &dest, int y, const plane &p )
{
	int sy1 = std::min( int(p.height() - 1), y + 1 );
	int sym1 = std::max( int(0), y - 1 );

	scanline p1 = scan_ref( p, sy1 );
	scanline c1 = scan_ref( p, y );
	scanline m1 = scan_ref( p, sym1 );

	float r1, r2, r3, r4, r5, r6;
	for ( int x = 0, w = dest.width(); x < w; ++x )
	{
		int xm1 = std::max( int(0), x - 1 );
		int xp1 = std::min( w - 1, x + 1 );

		r1 = m1[xm1]; r2 = m1[x]; r3 = m1[xp1];
		r4 = c1[xm1]; r5 = c1[x]; r6 = c1[xp1];
		mnmx6( r1, r2, r3, r4, r5, r6 );
		r1 = p1[xm1];
		mnmx5( r1, r2, r3, r4, r5 );
		r1 = p1[x];
		mnmx4( r1, r2, r3, r4 );
		r1 = p1[xp1];
		mnmx3( r1, r2, r3 );
		dest[x] = r2;
	}
}

////////////////////////////////////////

static void generic_median_thread( size_t , int s, int e, plane &r, const plane &p, int diam )
{
	std::vector<float> tmpV;
	tmpV.resize( static_cast<size_t>( diam * diam ) );

	int halfD = diam / 2;
	bool even = halfD * 2 == diam;
	int w = p.width();
	int h = p.height();
	int wm1 = w - 1;
	int hm1 = h - 1;
	size_t middle = tmpV.size() / 2;
	for ( int y = s; y < e; ++y )
	{
		int fy = y - halfD;
		int ty = y + halfD;
		if ( even )
			++fy;
		float *destP = r.line( y );
		for ( int x = 0; x < w; ++x )
		{
			size_t i = 0;

			int fx = x - halfD;
			int tx = x + halfD;
			if ( even )
				++fx;
			for ( int cy = fy; cy <= ty; ++cy )
			{
				int ready = std::min( hm1, std::max( int(0), cy ) );
				const float *lineP = p.line( ready );

				for ( int cx = fx; cx <= tx; ++cx )
				{
					int readx = std::min( wm1, std::max( int(0), cx ) );
					tmpV[i++] = lineP[readx];
				}
			}
			std::partial_sort( tmpV.begin(), tmpV.begin() + static_cast<long>( middle + 1 ), tmpV.end() );
			destP[x] = tmpV[middle];
		}
	}
}

static plane generic_median( const plane &p, int diam )
{
	plane r( p.width(), p.height() );

	threading::get().dispatch( std::bind( generic_median_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( r ), std::cref( p ), diam ), p );

	return r;
}

////////////////////////////////////////

void
cross_x_median( scanline &dest, int y, const plane &p )
{
	scanline s = scan_ref( p, y );
	if ( dest.width() < 5 )
	{
		for ( int x = 0; x != dest.width(); ++x )
			dest[x] = s[x];
		return;
	}
	int syp2 = std::min( int(p.height() - 1), y + 1 );
	int syp1 = std::min( int(p.height() - 1), y + 1 );
	int sym1 = std::max( int(0), y - 1 );
	int sym2 = std::max( int(0), y - 2 );

	scanline m2 = scan_ref( p, sym2 );
	scanline m1 = scan_ref( p, sym1 );
	scanline p1 = scan_ref( p, syp1 );
	scanline p2 = scan_ref( p, syp2 );

	float x1, x2, x3, x4, x5, x6, x7, x8, x9;
	float c1, c2, c3, c4, c5, c6, c7, c8, c9;
	float img;
	int wm1 = dest.width() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		int xm2 = std::max( int(0), x - 2 );
		int xm1 = std::max( int(0), x - 1 );
		int xp1 = std::min( wm1, x + 1 );
		int xp2 = std::min( wm1, x + 2 );
		x1 = m2[xm2];   c1 = m2[x];   x2 = m2[xp2];
		  x3 = m1[xm1]; c2 = m1[x]; x4 = m1[xp1];
		c3 = s[xm2];
		c4 = s[xm1];
		c5 = s[x];
		x5 = c5;
		img = c5;
		c6 = s[xp1];
		c7 = s[xp2];
		  x6 = p1[xm1]; c8 = p1[x]; x7 = p1[xp1];
		x8 = p2[xm2];   c9 = p2[x];   x9 = p2[xp2];
		mnmx9( c1, c2, c3, c4, c5, c6, c7, c8, c9 );
		mnmx9( x1, x2, x3, x4, x5, x6, x7, x8, x9 );
		mnmx3( img, c2, x2 );
		dest[x] = c2;
	}
}

////////////////////////////////////////

void
median_planes( scanline &dest, const scanline &a, const scanline &b, const scanline &c )
{
	for ( int x = 0, w = dest.width(); x < w; ++x )
	{
		float av = a[x];
		float bv = b[x];
		float cv = c[x];
		sort( av, bv );
		sort( bv, cv );
		sort( av, bv );
		dest[x] = bv;
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
	
	return plane( "p.sep_conv_v", p.dims(), p, k );
}

////////////////////////////////////////

plane
median( const plane &p, int diameter )
{
	if ( diameter == 3 )
	{
		return plane( "p.median_3x3", p.dims(), p );
	}

	return plane( "p.median", p.dims(), p, diameter );
}

////////////////////////////////////////

plane
cross_x_img_median( const plane &p )
{
	return plane( "p.cross_x_median", p.dims(), p );
}

////////////////////////////////////////

plane
median3( const plane &p1, const plane &p2, const plane &p3 )
{
	return plane( "p.median3", p1.dims(), p1, p2, p3 );
}

////////////////////////////////////////

void add_plane_area( engine::registry &r )
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

	r.add( op( "p.median_3x3", base::choose_runtime( median_3x3 ), n_scanline_plane_adapter<false, decltype(median_3x3)>(), dispatch_scan_processing, op::n_to_one ) );

	// rather than recreate a vector every scan, use threading
	r.add( op( "p.median", base::choose_runtime( generic_median ), op::threaded ) );

	r.add( op( "p.cross_x_median", base::choose_runtime( cross_x_median ), n_scanline_plane_adapter<false, decltype(cross_x_median)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.median3", base::choose_runtime( median_planes ), scanline_plane_adapter<true, decltype(median_planes)>(), dispatch_scan_processing, op::n_to_one ) );

}

////////////////////////////////////////
} // image



