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

#include "spatial_filter.h"
#include <base/math_functions.h>
#include <base/contract.h>
#include <base/cpu_features.h>
#include "plane_ops.h"
#include "scanline_process.h"
#include "threading.h"

////////////////////////////////////////

namespace
{
using namespace image;


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

////////////////////////////////////////

static void
cross_bilateral_thread( size_t , int s, int e, plane &r, const plane &p, const plane &ref, int dx, int dy, float sigR, float sigI )
{
	std::vector<float> sumW;
	sumW.resize( static_cast<size_t>( p.width() ), 0.F );

	int w = p.width();
	int hm1 = p.height() - 1;
	int wm1 = w - 1;
	float dsig = -1.F / ( sigR * sigR * 2.F );
	float isig = -1.F / ( sigI * sigI * 2.F );
	for ( int y = s; y < e; ++y )
	{
		float *destP = r.line( y );
		for ( int x = 0; x < w; ++x )
			destP[x] = 0.F;

		const float *cenP = ref.line( y );
		for ( int cy = y - dy; cy <= y + dy; ++cy )
		{
			int rY = std::max( int(0), std::min( hm1, cy ) );
			int distY = (cy - y) * (cy - y);
			const float *srcP = p.line( rY );
			const float *refP = ref.line( rY );
			for ( int x = 0; x < w; ++x )
			{
				float cenV = cenP[x];
				for ( int cx = x - dx; cx <= x + dx; ++cx )
				{
					int rX = std::max( int(0), std::min( wm1, cx ) );
					int distX = (cx - x) * (cx - x);
					float oV = refP[rX];
					float weight = expf( static_cast<float>( distY + distX ) * dsig + ( (oV - cenV) * (oV - cenV ) ) * isig );
					sumW[static_cast<size_t>(x)] += weight;
					destP[x] += weight * srcP[rX];
				}
			}
		}

		for ( int x = 0; x < w; ++x )
		{
			float &weight = sumW[static_cast<size_t>(x)];
			destP[x] /= weight;
			weight = 0.F;
		}
	}
}

static plane
apply_bilateral( const plane &p, int dx, int dy, float sigR, float sigI )
{
	plane r( p.width(), p.height() );

	threading::get().dispatch( std::bind( cross_bilateral_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( r ), std::cref( p ), std::cref( p ), dx, dy, sigR, sigI ), p );

	return r;
}

static plane
apply_cross_bilateral( const plane &p, const plane &ref, int dx, int dy, float sigR, float sigI )
{
	plane r( p.width(), p.height() );

	threading::get().dispatch( std::bind( cross_bilateral_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( r ), std::cref( p ), std::cref( ref ), dx, dy, sigR, sigI ), p );

	return r;
}

////////////////////////////////////////

static void
weighted_bilateral_thread( size_t , int s, int e, plane &r, const plane &p, const plane &weight, int dx, int dy, float sigR, float sigI )
{
	std::vector<float> sumW;
	sumW.resize( static_cast<size_t>( p.width() ), 0.F );

	int w = p.width();
	int hm1 = p.height() - 1;
	int wm1 = w - 1;
	float dsig = -1.F / ( sigR * sigR * 2.F );
	for ( int y = s; y < e; ++y )
	{
		float *destP = r.line( y );
		for ( int x = 0; x < w; ++x )
			destP[x] = 0.F;

		const float *weightP = weight.line( y );
		const float *cenP = p.line( y );
		for ( int cy = y - dy; cy <= y + dy; ++cy )
		{
			int rY = std::max( int(0), std::min( hm1, cy ) );
			int distY = (cy - y) * (cy - y);
			const float *srcP = p.line( rY );
			for ( int x = 0; x < w; ++x )
			{
				float cenV = cenP[x];
				float tsig = sigI * weightP[x];
				float isig = -1.F / ( tsig * tsig * 2.F );
				for ( int cx = x - dx; cx <= x + dx; ++cx )
				{
					int rX = std::max( int(0), std::min( wm1, cx ) );
					int distX = (cx - x) * (cx - x);
					float oV = srcP[rX];
					float weightV = expf( static_cast<float>( distY + distX ) * dsig + ( (oV - cenV) * (oV - cenV ) ) * isig );
					sumW[static_cast<size_t>(x)] += weightV;
					destP[x] += weightV * srcP[rX];
				}
			}
		}

		for ( int x = 0; x < w; ++x )
		{
			float &weightV = sumW[static_cast<size_t>(x)];
			destP[x] /= weightV;
			weightV = 0.F;
		}
	}
}

static plane
apply_weighted_bilateral( const plane &p, const plane &w, int dx, int dy, float sigR, float sigI )
{
	plane r( p.width(), p.height() );

	threading::get().dispatch( std::bind( weighted_bilateral_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( r ), std::cref( p ), std::cref( w ), dx, dy, sigR, sigI ), p );

	return r;
}

////////////////////////////////////////

static inline std::tuple<plane, plane, plane, plane>
wavelet_decomp( const plane &p, const std::vector<float> &h, const std::vector<float> &g )
{
	plane hhc = convolve_vert( p, h );
	plane ghc = convolve_vert( p, g );
	plane c_j1 = convolve_horiz( hhc, h );
	plane w1_j1 = convolve_horiz( hhc, g );
	plane w2_j1 = convolve_horiz( ghc, h );
	plane w3_j1 = convolve_horiz( ghc, g );

	return std::make_tuple( c_j1, w1_j1, w2_j1, w3_j1 );
}

//inline plane
//logistic( const plane &x, float x0, float k = 1.F, float L = 1.F )
//{
//	return L / ( 1.F + exp( (-k) * ( x - x0 ) ) );
//}

inline plane
gaussian( const plane &x, float a, float b, float c )
{
	return a * exp( abs( x - b ) / ( -2.F * c * c ) );
}

inline plane
gaussian( const plane &x, float a, float b, const plane &sigma )
{
	return a * exp( abs( x - b ) / ( -2.F * square( sigma ) ) );
}

template <typename Sigma>
plane
wavelet_filter_impl( const plane &p, size_t levels, Sigma sigma )
{
	precondition( levels > 0, "invalid levels {0}", levels );
	std::vector<std::tuple<plane, plane, plane>> filtLevels;

	std::vector<float> wt_h{ 1.F/16.F, 4.F/16.F, 6.F/16.F, 4.F/16.F, 1.F/16.F };
	std::vector<float> wt_g = base::dirac_negate( wt_h );

	plane c_J = p;
	size_t cnt = levels;
	while ( true )
	{
		auto wd = wavelet_decomp( c_J, wt_h, wt_g );
		c_J = std::get<0>( wd );
		filtLevels.push_back( std::make_tuple( std::get<1>( wd ), std::get<2>( wd ), std::get<3>( wd ) ) );
		if ( cnt == 0 )
			break;

		--cnt;
		wt_h = base::atrous_expand( wt_h );
		wt_g = base::atrous_expand( wt_g );
	}

	postcondition( filtLevels.size() == (levels + 1), "Expecting {0} levels", (levels + 1) );

	float levelScale = 6.F/16.F;
	for ( size_t l = 0; l < levels; ++l )
	{
		auto &curL = filtLevels[l];
		// todo: add next level correlation
//		auto &nextL = filtLevels[l+1];
		plane &curPh = std::get<0>( curL );
		plane &curPv = std::get<1>( curL );
		plane &curPc = std::get<2>( curL );

		plane s1 = ( 1.F - gaussian( curPh, 1.F, 0.F, sigma ) );
		plane s2 = ( 1.F - gaussian( curPv, 1.F, 0.F, sigma ) );
		plane s3 = ( 1.F - gaussian( curPc, 1.F, 0.F, sigma ) );

		plane w = max( s1, max( s2, s3 ) );
		curPh = curPh * w;
		curPv = curPv * w;
		curPc = curPc * w;

		sigma *= levelScale;
	}

	plane reconst = c_J;
	for ( auto &l: filtLevels )
		reconst += std::get<0>( l ) + std::get<1>( l ) + std::get<2>( l );

	return reconst;
}

template <typename Epsilon>
inline plane
guided_filter_impl( const plane &I, const plane &p, int r, Epsilon eps )
{
	precondition( p.width() == I.width() && p.height() == I.height(), "unable to guided_filter planes of different sizes" );
	plane mean_I = local_mean( I, r );
//	plane mean_II = local_mean( square( I ), r );
//	plane var_I = mean_II - square( mean_I );
	plane var_I = local_variance( I, r );

	plane mean_p = local_mean( p, r );
	plane mean_Ip = local_mean( I * p, r );
	plane cov_Ip = mean_Ip - mean_I * mean_p;

	plane a = cov_Ip / ( var_I + eps );
	plane b = mean_p - a * mean_I;

	plane mean_a = local_mean( a, r );
	plane mean_b = local_mean( b, r );
	return mean_a * I + mean_b;
}


template <typename Epsilon>
inline image_buf
guided_filter_color_impl( const image_buf &I, const image_buf &p, int r, Epsilon eps )
{
	image_buf ret = p;
	if ( I.size() >= 3 && p.size() >= 3 )
	{
		plane mean_I_r = local_mean( I[0], r );
		plane mean_I_g = local_mean( I[1], r );
		plane mean_I_b = local_mean( I[2], r );

		// variance becomes a matrix
		// [ rr rg rb
		//   rg gg gb
		//   rb gb bb ]
		plane var_I_rr = local_mean( I[0] * I[0], r ) - mean_I_r * mean_I_r + eps;
		plane var_I_rg = local_mean( I[0] * I[1], r ) - mean_I_r * mean_I_g;
		plane var_I_rb = local_mean( I[0] * I[2], r ) - mean_I_r * mean_I_b;
		plane var_I_gg = local_mean( I[1] * I[1], r ) - mean_I_g * mean_I_g + eps;
		plane var_I_gb = local_mean( I[1] * I[2], r ) - mean_I_g * mean_I_b;
		plane var_I_bb = local_mean( I[2] * I[2], r ) - mean_I_b * mean_I_b + eps;

		plane invrr = var_I_gg * var_I_bb - var_I_gb * var_I_gb;
		plane invrg = var_I_gb * var_I_rb - var_I_rg * var_I_bb;
		plane invrb = var_I_rg * var_I_gb - var_I_gg * var_I_rb;
		plane invgg = var_I_rr * var_I_bb - var_I_rb * var_I_rb;
		plane invgb = var_I_rb * var_I_rg - var_I_rr * var_I_gb;
		plane invbb = var_I_rr * var_I_gg - var_I_rg * var_I_rg;

		plane det = invrr * var_I_rr + invrg * var_I_rg + invrb * var_I_rb;
		invrr /= det;
		invrg /= det;
		invrb /= det;
		invgg /= det;
		invgb /= det;
		invbb /= det;

		for ( size_t i = 0, N = ret.size(); i != N; ++i )
		{
			plane mean_p = local_mean( p[i], r );
			plane mean_Ip_r = local_mean( I[0] * p[i], r );
			plane mean_Ip_g = local_mean( I[1] * p[i], r );
			plane mean_Ip_b = local_mean( I[2] * p[i], r );
			plane cov_Ip_r = mean_Ip_r - mean_I_r * mean_p;
			plane cov_Ip_g = mean_Ip_g - mean_I_g * mean_p;
			plane cov_Ip_b = mean_Ip_b - mean_I_b * mean_p;
			plane a_r = invrr * cov_Ip_r + invrg * cov_Ip_g + invrb * cov_Ip_b;
			plane a_g = invrg * cov_Ip_r + invgg * cov_Ip_g + invgb * cov_Ip_b;
			plane a_b = invrb * cov_Ip_r + invgb * cov_Ip_g + invbb * cov_Ip_b;
			plane b = mean_p - a_r * mean_I_r - a_g * mean_I_g - a_b * mean_I_b;

			ret[i] = local_mean( a_r, r ) * I[0] + local_mean( a_g, r ) * I[1] + local_mean( a_b, r ) * I[2] + local_mean( b, r );
		}
	}
	else
	{
		precondition( I.size() >= p.size(), "mismatch in plane count" );
		for ( size_t i = 0, N = ret.size(); i != N; ++i )
			ret[i] = guided_filter_mono( I[i], p[i], r, eps );
	}

	return ret;
}

}

////////////////////////////////////////

namespace image
{

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

plane
despeckle( const plane &p, float thresh )
{
	plane mid = separable_convolve( p, { 0.25F, 0.5F, 0.25F } );
	plane high = p - mid;
	plane med = cross_x_img_median( p );
	plane highmed = p - med;
	plane highdiff = high - highmed;
	return mid + high * ( 1.F - exp( square( highdiff ) / ( -2.F * thresh * thresh ) ) );
}

////////////////////////////////////////

plane
bilateral( const plane &p1, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI )
{
	return plane( "p.bilateral", p1.dims(), p1, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane
cross_bilateral( const plane &p1, const plane &ref, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI )
{
	return plane( "p.cross_bilateral", p1.dims(), p1, ref, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane
weighted_bilateral( const plane &p1, const plane &w, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI )
{
	return plane( "p.weighted_bilateral", p1.dims(), p1, w, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane
wavelet_filter( const plane &p, size_t levels, float sigma )
{
	return wavelet_filter_impl( p, levels, sigma );
}

plane
wavelet_filter( const plane &p, size_t levels, const plane &sigma )
{
	precondition( p.width() == sigma.width() && p.height() == sigma.height(), "unable to wavelet_filter planes of different sizes" );
	return wavelet_filter_impl( p, levels, sigma );
}

////////////////////////////////////////

plane
guided_filter_mono( const plane &I, const plane &p, int r, float eps )
{
	return guided_filter_impl( I, p, r, eps );
}

plane
guided_filter_mono( const plane &I, const plane &p, int r, const plane &eps, float epsScale )
{
	precondition( p.width() == eps.width() && p.height() == eps.height(), "unable to guided_filter planes of different sizes" );
	return guided_filter_impl( I, p, r, eps * epsScale );
}

////////////////////////////////////////

image_buf
guided_filter_mono( const image_buf &I, const image_buf &p, int r, float eps )
{
	image_buf ret = I;
	for ( size_t c = 0; c < ret.size(); ++c )
		ret[c] = guided_filter_mono( I[c], p[c], r, eps );
	return ret;
}

image_buf
guided_filter_mono( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale )
{
	image_buf ret = I;
	plane e = eps * epsScale;
	for ( size_t c = 0; c < ret.size(); ++c )
		ret[c] = guided_filter_mono( I[c], p[c], r, e );
	return ret;
}

////////////////////////////////////////

image_buf
guided_filter_color( const image_buf &I, const image_buf &p, int r, float eps )
{
	return guided_filter_color_impl( I, p, r, eps );
}

image_buf
guided_filter_color( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale )
{
	return guided_filter_color_impl( I, p, r, eps * epsScale );
}

////////////////////////////////////////

void add_spatial( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.median_3x3", base::choose_runtime( median_3x3 ), n_scanline_plane_adapter<false, decltype(median_3x3)>(), dispatch_scan_processing, op::n_to_one ) );

	// rather than recreate a vector every scan, use threading
	r.add( op( "p.median", base::choose_runtime( generic_median ), op::threaded ) );

	r.add( op( "p.cross_x_median", base::choose_runtime( cross_x_median ), n_scanline_plane_adapter<false, decltype(cross_x_median)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.median3", base::choose_runtime( median_planes ), scanline_plane_adapter<true, decltype(median_planes)>(), dispatch_scan_processing, op::one_to_one ) );

	// wants a temporary scanline for efficiency, so just do generic threading
	r.add( op( "p.bilateral", base::choose_runtime( apply_bilateral ), op::threaded ) );
	r.add( op( "p.cross_bilateral", base::choose_runtime( apply_cross_bilateral ), op::threaded ) );
	r.add( op( "p.weighted_bilateral", base::choose_runtime( apply_weighted_bilateral ), op::threaded ) );
}

////////////////////////////////////////

} // namespace image

