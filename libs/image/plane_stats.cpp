//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "plane_stats.h"
#include "threading.h"
#include "scanline_process.h"
#include <base/cpu_features.h>
#include <base/contract.h>

#include "sse3/plane_stats.h"

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
	int y0 = y - radius - 1;
	int y1 = std::max( sat.y1(), std::min( sat.y2(), y + radius ) );
	int nY = y1 - (y0 < sat.y1() ? -1 : y0 );
	const double *y1line = sat.line( y1 );
	if ( y0 < sat.y1() )
	{
		for ( int x = 0; x <= wm1; ++x )
		{
			int x0 = x - radius - 1;
			int x1 = std::max( int(0), std::min( wm1, x + radius ) );
			int nX = x1 - (x0 < 0 ? -1 : x0);
			double C = 0.0;
			if ( x0 >= 0 )
				C = y1line[x0];
			double D = y1line[x1];
			double sum = D - C;
			dest[x] = static_cast<float>( sum / static_cast<double>( nX * nY ) );
		}
	}
	else
	{
		const double *y0line = sat.line( y0 );
		int x1 = std::min( wm1, radius );
		for ( int x = 0; x <= radius; ++x )
		{
			int nX = x1 + 1;
			double B = y0line[x1];
			double D = y1line[x1];
			double sum = D - B;
			dest[x] = static_cast<float>( sum / static_cast<double>( nX * nY ) );
			x1 = std::min( wm1, x1 + 1 );
		}
		int x0 = 0;
		for ( int x = (radius + 1); x <= wm1; ++x, ++x0 )
		{
			int nX = x1 - x0;
			double A = y0line[x0];
			double B = y0line[x1];
			double C = y1line[x0];
			double D = y1line[x1];
			double sum = A + D - B - C;
			dest[x] = static_cast<float>( sum / static_cast<double>( nX * nY ) );
			x1 = std::min( wm1, x1 + 1 );
		}
	}
}

static void
compute_mean( scanline &dest, int y, const plane &p, int radius )
{
	int64_t scaleI = ( radius * 2 + 1 ) * ( radius * 2 + 1 );
	int minY = y - radius, maxY = y + radius;
	for ( int cy = minY; cy <= (y+radius); ++cy )
	{
		if ( cy < p.y1() )
		{
			++minY;
			scaleI -= ( radius * 2 + 1 );
		}
		if ( cy > p.y2() )
		{
			--maxY;
			scaleI -= ( radius * 2 + 1 );
		}
	}
	int wm1 = dest.width() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		double sum = 0.0;

		int64_t xscale = scaleI;
		int minX = x - radius;
		int maxX = x + radius;
		if ( minX < 0 )
		{
			xscale += minX * ( maxY - minY + 1 );
			minX = 0;
		}
		if ( maxX > wm1 )
		{
			xscale -= ( maxX - wm1 ) * ( maxY - minY + 1 );
			maxX = wm1;
		}
		double scale = static_cast<double>( xscale );
		for ( int cy = minY; cy <= maxY; ++cy )
		{
			const float *srcL = p.line( cy );
			for ( int cx = minX; cx <= maxX; ++cx )
				sum += static_cast<double>( srcL[cx] );
		}
		dest[x] = static_cast<float>( sum / scale );
	}
}

////////////////////////////////////////

static void
compute_variance_sat( scanline &dest, int y, const accum_buf &sat, const accum_buf &sat2, int radius )
{
	int wm1 = sat.width() - 1;
	int y0 = y - radius - 1;
	int y1 = std::max( sat.y1(), std::min( sat.y2(), y + radius ) );
	int nY = y1 - (y0 < sat.y1() ? -1 : y0 );
	for ( int x = 0; x <= wm1; ++x )
	{
		int x0 = x - radius - 1;
		int x1 = std::max( int(0), std::min( wm1, x + radius ) );
		int nX = x1 - (x0 < 0 ? -1 : x0);
		double Am = 0.0, Bm = 0.0, Cm = 0.0;
		double As = 0.0, Bs = 0.0, Cs = 0.0;
		if ( y0 >= sat.y1() )
		{
			Bm = sat.get( x1 + sat.x1(), y0 );
			Bs = sat2.get( x1 + sat.x1(), y0 );
			if ( x0 >= 0 )
			{
				Am = sat.get( x0 + sat.x1(), y0 );
				As = sat2.get( x0 + sat.x1(), y0 );
			}
		}
		if ( x0 >= 0 )
		{
			Cm = sat.get( x0 + sat.x1(), y1 );
			Cs = sat2.get( x0 + sat.x1(), y1 );
		}
		double Dm = sat.get( x1 + sat.x1(), y1 );
		double Ds = sat2.get( x1 + sat.x1(), y1 );
		
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
	for ( int x = 0; x <= wm1; ++x )
	{
		double K = 0.0;
		double ex = 0.0;
		double ex2 = 0.0;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			int rY = std::max( p.y1(), std::min( p.y2(), cy ) );
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
		double *out = dest.data() + ( x - p.x1() );
		const float *col = p.cdata() + ( x - p.x1() );
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
	accum_buf dest( p.x1(), p.y1(), p.x2(), p.y2() );
	threading::get().dispatch( std::bind( sat_cols, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ), std::cref( p ), power ), p.x1(), p.width() );

	threading::get().dispatch( std::bind( sat_rows, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ) ), p.y1(), dest.height() );

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
	add_histo( 0, p.y1(), p.y2(), p, vals, lowV, highV );
	return vals[0];
}

////////////////////////////////////////

static void
compute_mse( scanline &dest, int y, const plane &p, const plane &p2, int radius )
{
	int wm1 = dest.width() - 1;
	int n2 = radius * 2 + 1;
	n2 *= n2;
	float norm = 1.F / static_cast<float>( n2 );
	for ( int x = 0; x <= wm1; ++x )
	{
		float sum = 0.F;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			int rY = std::max( p.y1(), std::min( p.y2(), cy ) );
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
	engine::dimensions d = sat.dims();
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(float) );
	return plane( "p.local_mean_sat", d, sat, radius );
}

////////////////////////////////////////

plane
local_variance( const accum_buf &sat, const accum_buf &sat2, int radius )
{
	engine::dimensions d = sat.dims();
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(float) );
	return plane( "p.local_variance_sat", d, sat, sat2, radius );
}

////////////////////////////////////////

accum_buf
sum_area_table( const plane &p, int power )
{
	engine::dimensions d = p.dims();
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(double) );
	return accum_buf( "p.sum_area_table", d, p, power );
}

////////////////////////////////////////

plane
mse( const plane &p, const plane &p2, int radius )
{
	precondition( p.dims() == p2.dims(), "unable to compute MSE for planes of different sizes" );
	return plane( "p.mean_square_error", p.dims(), p, p2, radius );
}

////////////////////////////////////////

engine::computed_value< std::vector<uint64_t> >
histogram( const plane &p, int bins, float lowV, float highV )
{
	engine::dimensions d;
	d.x1 = 0;
	d.y1 = 0;
	d.x2 = bins - 1;
	d.y2 = 0;
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(uint64_t) );
	return engine::computed_value< std::vector<uint64_t> >( op_registry(), "p.histogram", d, p, bins, lowV, highV );
}

////////////////////////////////////////

void
add_plane_stats( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.sum", sum_plane, op::threaded ) );
	r.add( op( "p.sum_area_table", base::choose_runtime( compute_SAT, { { base::cpu::simd_feature::SSE3, sse3::compute_SAT } } ), op::threaded ) );
	r.add( op( "p.histogram", compute_histogram, op::threaded ) );

	// methods using the summed area table
	r.add( op( "p.local_mean_sat", base::choose_runtime( compute_mean_sat ), n_scanline_plane_adapter<false, decltype(compute_mean_sat)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.local_variance_sat", base::choose_runtime( compute_variance_sat ), n_scanline_plane_adapter<false, decltype(compute_variance_sat)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.local_mean", base::choose_runtime( compute_mean ), n_scanline_plane_adapter<false, decltype(compute_mean)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.local_variance", base::choose_runtime( compute_variance ), n_scanline_plane_adapter<false, decltype(compute_variance)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "p.mean_square_error", base::choose_runtime( compute_mse ), n_scanline_plane_adapter<false, decltype(compute_mse)>(), dispatch_scan_processing, op::n_to_one ) );
}

} // image



