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

#include "plane_stats.h"
#include "threading.h"
#include "scanline_process.h"
#include <base/cpu_features.h>
#include <base/contract.h>

////////////////////////////////////////

namespace
{
using namespace image;

static void sum_thread( size_t tIdx, int s, int e, const plane &p, std::vector<double> &vals )
{
	double v = 0.0;
	int w = p.width();
	for ( int y = s; y < e; ++y )
	{
		const float *lineP = p.line( y );
		for ( int x = 0; x < w; ++x )
			v += static_cast<double>( lineP[x] );
	}
	vals[tIdx] += v;
}

static double sum_plane( const plane &p )
{
	std::vector<double> vals;
	vals.resize( threading::get().size(), 0.0 );

	threading::get().dispatch( std::bind( sum_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::cref( p ), std::ref( vals ) ), p );

	double r = 0.0;
	for ( double d: vals )
		r += d;
	return r;
}

static void
compute_mean_sat( scanline &dest, int y, const accum_buf &sat, int radius )
{
	int wm1 = sat.width() - 1;
	int hm1 = sat.height() - 1;
	int y0 = y - radius - 1;
	int y1 = std::max( 0, std::min( hm1, y + radius ) );
	int nY = y1 - y0;
	for ( int x = 0; x <= wm1; ++x )
	{
		int x0 = x - radius - 1;
		int x1 = std::max( int(0), std::min( wm1, x + radius ) );
		int nX = x1 - x0;
		double A = 0.0, B = 0.0, C = 0.0;
		if ( y0 >= 0 )
		{
			B = sat.get( x1, y0 );
			if ( x0 >= 0 )
				A = sat.get( x0, y0 );
		}
		if ( x0 >= 0 )
			C = sat.get( x0, y1 );
		double D = sat.get( x1, y1 );
		double sum = A + D - B - C;
		dest[x] = static_cast<float>( sum / static_cast<double>( nX * nY ) );
	}
}

static void
compute_mean( scanline &dest, int y, const plane &p, int radius )
{
	double scale = static_cast<double>( ( radius * 2 + 1 ) * ( radius * 2 + 1 ) );
	int wm1 = dest.width() - 1;
	int hm1 = p.height() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		double sum = 0.0;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			int rY = std::max( int(0), std::min( hm1, cy ) );
			const float *srcL = p.line( rY );
			for ( int cx = x - radius; cx <= x + radius; ++cx )
			{
				int rx = std::max( int(0), std::min( wm1, cx ) );
				sum += static_cast<double>( srcL[rx] );
			}
		}
		dest[x] = static_cast<float>( sum / scale );
	}
}

////////////////////////////////////////

static void
compute_variance_sat( scanline &dest, int y, const accum_buf &sat, const accum_buf &sat2, int radius )
{
	int wm1 = sat.width() - 1;
	int hm1 = sat.height() - 1;
	int y0 = y - radius - 1;
	int y1 = std::max( 0, std::min( hm1, y + radius ) );
	int nY = y1 - y0;
	for ( int x = 0; x <= wm1; ++x )
	{
		int x0 = x - radius - 1;
		int x1 = std::max( int(0), std::min( wm1, x + radius ) );
		int nX = x1 - x0;
		double Am = 0.0, Bm = 0.0, Cm = 0.0;
		double As = 0.0, Bs = 0.0, Cs = 0.0;
		if ( y0 >= 0 )
		{
			Bm = sat.get( x1, y0 );
			Bs = sat2.get( x1, y0 );
			if ( x0 >= 0 )
			{
				Am = sat.get( x0, y0 );
				As = sat2.get( x0, y0 );
			}
		}
		if ( x0 >= 0 )
		{
			Cm = sat.get( x0, y1 );
			Cs = sat2.get( x0, y1 );
		}
		double Dm = sat.get( x1, y1 );
		double Ds = sat2.get( x1, y1 );
		
		double Ex = Am + Dm - Bm - Cm;
		double Ex2 = As + Ds - Bs - Cs;
		int n = nX * nY;
		double v = ( Ex2 - (Ex*Ex) / static_cast<double>( n ) ) / static_cast<double>( n - 1 );
		dest[x] = static_cast<float>( v );
	}
}

static void
compute_variance( scanline &dest, int y, const plane &p, int radius )
{
	// avoid the cancellation problem by using the first input value
	// as an offset. if this proves unreliable, have to make two
	// passes :(
	const double n = static_cast<double>( ( radius * 2 + 1 ) * ( radius * 2 + 1 ) );
	int wm1 = dest.width() - 1;
	int hm1 = p.height() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		double K = 0.0;
		double ex = 0.0;
		double ex2 = 0.0;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			int rY = std::max( int(0), std::min( hm1, cy ) );
			const float *srcL = p.line( rY );
			if ( cy == (y - radius) )
				K = static_cast<double>( srcL[std::max(int(0), x - radius)] );
			for ( int cx = x - radius; cx <= x + radius; ++cx )
			{
				int rx = std::max( int(0), std::min( wm1, cx ) );
				double v = static_cast<double>( srcL[rx] ) - K;
				ex += v;
				ex2 += v * v;
			}
		}
		double v = ( ex2 - (ex*ex) / n ) / ( n - 1.0 );
		dest[x] = static_cast<float>( v );
	}
}

////////////////////////////////////////

static void sat_cols( size_t, int s, int e, accum_buf &dest, const plane &p, int power )
{
	int dS = dest.stride();
	int pS = p.stride();
	int h = dest.height();
	for ( int x = s; x < e; ++x )
	{
		double sum = 0.0;
		double *out = dest.data() + x;
		const float *col = p.cdata() + x;
		switch ( power )
		{
			case 1:
				for ( int c = 0; c < h; ++c, out += dS, col += pS )
				{
					sum += static_cast<double>( *col );
					*out = sum;
				}
				break;
			case 2:
				for ( int c = 0; c < h; ++c, out += dS, col += pS )
				{
					double cv = static_cast<double>( *col );
					sum += cv * cv;
					*out = sum;
				}
				break;
			case 3:
				for ( int c = 0; c < h; ++c, out += dS, col += pS )
				{
					double cv = static_cast<double>( *col );
					sum += cv * cv * cv;
					*out = sum;
				}
				break;
			case 4:
				for ( int c = 0; c < h; ++c, out += dS, col += pS )
				{
					double cv = static_cast<double>( *col );
					sum += cv * cv * cv * cv;
					*out = sum;
				}
				break;
			default:
				for ( int c = 0; c < h; ++c, out += dS, col += pS )
				{
					sum += pow( static_cast<double>( *col ), power );
					*out = sum;
				}
				break;
		}
	}
}

static void sat_rows( size_t, int s, int e, accum_buf &dest )
{
	int w = dest.width();
	for ( int y = s; y < e; ++y )
	{
		double *out = dest.line( y );
		double sum = 0.0;
		for ( int x = 0; x < w; ++x )
		{
			sum += out[x];
			out[x] = sum;
		}
	}
}

// summed area table is p(x,y) + SAT(x-1,y) + SAT(x,y-1) - SAT(x-1,y-1)
//
// if you compute a column
//
// SAT(x,y) = p(x,y) + SAT(x,y-1)
//
// then you make a horizontal pass, you end up with the summed area
// table formulation which allows arbitrary window manipulation after
// for computing windowed sums
static accum_buf
compute_SAT( const plane &p, int power )
{
	accum_buf dest( p.width(), p.height() );
	threading::get().dispatch( std::bind( sat_cols, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ), std::cref( p ), power ), 0, p.width() );

	threading::get().dispatch( std::bind( sat_rows, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ) ), 0, dest.height() );

	return dest;
}

////////////////////////////////////////

static void
add_histo( size_t tIdx, int s, int e, const plane &p, std::vector<std::vector<uint64_t>> &vals, float lowV, float highV )
{
	std::vector<uint64_t> &histo = vals[tIdx];
	float binScale = static_cast<float>( histo.size() - 1 );
	int w = p.width();
	for ( int y = s; y < e; ++y )
	{
		const float *in = p.line( y );
		for ( int x = 0; x < w; ++x )
		{
			float v = binScale * ( in[x] - lowV ) / ( highV - lowV );
			size_t idx = static_cast<size_t>( std::max( 0.F, std::min( binScale, v ) ) );
			++histo[idx];
		}
	}
}

static std::vector<uint64_t>
compute_histogram( const plane &p, int bins, float lowV, float highV )
{
	size_t n = threading::get().size();
	std::vector<std::vector<uint64_t>> vals;
	if ( n > 1 )
	{
		size_t nThread = threading::get().size();
		vals.resize( nThread, std::vector<uint64_t>( static_cast<size_t>( bins ), 0 ) );
		
		threading::get().dispatch( std::bind( add_histo, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::cref( p ), std::ref( vals ), lowV, highV ), p );

		std::vector<uint64_t> &ret = vals[0];
		for ( size_t i = 1; i < nThread; ++i )
		{
			const std::vector<uint64_t> &cur = vals[i];
			for ( size_t b = 0; b != static_cast<size_t>( bins ); ++b )
				ret[b] += cur[b];
		}
		return ret;
	}

	vals.resize( 1, std::vector<uint64_t>( static_cast<size_t>( bins ), 0 ) );
	add_histo( 0, 0, p.height(), p, vals, lowV, highV );
	return vals[0];
}

////////////////////////////////////////

static void
compute_mse( scanline &dest, int y, const plane &p, const plane &p2, int radius )
{
	int wm1 = dest.width() - 1;
	int hm1 = p.height() - 1;
	int n2 = radius * 2 + 1;
	n2 *= n2;
	float norm = 1.F / static_cast<float>( n2 );
	for ( int x = 0; x <= wm1; ++x )
	{
		float sum = 0.F;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			int rY = std::max( int(0), std::min( hm1, cy ) );
			const float *srcL = p.line( rY );
			const float *srcL2 = p2.line( rY );

			for ( int cx = x - radius; cx <= x + radius; ++cx )
			{
				int rx = std::max( int(0), std::min( wm1, cx ) );
				float v = srcL[rx] - srcL2[rx];
				sum += v * v;
			}
		}
		dest[x] = sum * norm;
	}
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

TODO( "check breakover point where local ops are worth the summed area table optimization" )

plane
local_mean( const plane &p, int radius )
{
	// worth the extra buffer?
	if ( radius < 2 )
		return plane( "p.local_mean", p.dims(), p, radius );

	return local_mean( sum_area_table( p, 1 ), radius );
}

////////////////////////////////////////

plane
local_variance( const plane &p, int radius )
{
	if ( radius < 2 )
		return plane( "p.local_variance", p.dims(), p, radius );

	return local_variance( sum_area_table( p, 1 ), sum_area_table( p, 2 ), radius );
}

////////////////////////////////////////

plane
local_mean( const accum_buf &sat, int radius )
{
	return plane( "p.local_mean_sat", sat.dims(), sat, radius );
}

////////////////////////////////////////

plane
local_variance( const accum_buf &sat, const accum_buf &sat2, int radius )
{
	return plane( "p.local_variance_sat", sat.dims(), sat, sat2, radius );
}

////////////////////////////////////////

accum_buf
sum_area_table( const plane &p, int power )
{
	return accum_buf( "p.sum_area_table", p.dims(), p, power );
}

////////////////////////////////////////

plane
mse( const plane &p, const plane &p2, int radius )
{
	precondition( p.width() == p2.width() && p.height() == p2.height(), "unable to compute MSE for planes of different sizes" );
	return plane( "p.mean_square_error", p.dims(), p, p2, radius );
}

////////////////////////////////////////

engine::computed_value< std::vector<uint64_t> >
histogram( const plane &p, int bins, float lowV, float highV )
{
	engine::dimensions d;
	d.x = bins;
	return engine::computed_value< std::vector<uint64_t> >( op_registry(), "p.histogram", d, p, bins, lowV, highV );
}

////////////////////////////////////////

void
add_plane_stats( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.sum", sum_plane, op::threaded ) );
	r.add( op( "p.sum_area_table", compute_SAT, op::threaded ) );
	r.add( op( "p.histogram", compute_histogram, op::threaded ) );

	// methods using the summed area table
	r.add( op( "p.local_mean_sat", base::choose_runtime( compute_mean_sat ), n_scanline_plane_adapter<false, decltype(compute_mean_sat)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.local_variance_sat", base::choose_runtime( compute_variance_sat ), n_scanline_plane_adapter<false, decltype(compute_variance_sat)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.local_mean", base::choose_runtime( compute_mean ), n_scanline_plane_adapter<false, decltype(compute_mean)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.local_variance", base::choose_runtime( compute_variance ), n_scanline_plane_adapter<false, decltype(compute_variance)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.mean_square_error", base::choose_runtime( compute_mse ), n_scanline_plane_adapter<false, decltype(compute_mse)>(), dispatch_scan_processing, op::n_to_one ) );
}

} // image



