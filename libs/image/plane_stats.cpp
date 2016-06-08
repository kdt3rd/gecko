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
compute_variance( scanline &dest, int y, const plane &p, int radius )
{
	// avoid the cancellation problem by using the first input value
	// as an offset. if this proves unreliable, have to make two
	// passes :(
	int wm1 = dest.width() - 1;
	for ( int x = 0; x <= wm1; ++x )
	{
		double K = 0.0;
		double ex = 0.0;
		double ex2 = 0.0;
		for ( int cy = y - radius; cy <= (y+radius); ++cy )
		{
			const float *srcL = p.line( cy );
			if ( cy == (y - radius) )
				K = static_cast<double>( srcL[std::max(int(0), x - radius)] );
			for ( int cx = x - radius; cx <= x + radius; ++cx )
			{
				int rx = std::max( int(0), std::min( wm1, cx ) );
				double v = static_cast<double>( srcL[rx] ) - K;
				ex += v;
				ex += v * v;
			}
		}
		double n = static_cast<double>( radius * 2 + 1 );
		n *= n;
		double v = ( ex2 - (ex*ex) / n ) / ( n - 1.0 );
		dest[x] = static_cast<float>( v );
	}
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane
local_variance( const plane &p, int radius )
{
	return plane( "p.local_variance", p.dims(), p, radius );
}

////////////////////////////////////////

void
add_plane_stats( engine::registry &r )
{
	using namespace engine;

	r.add( op( "p.sum", sum_plane, op::threaded ) );

	r.add( op( "p.local_variance", base::choose_runtime( compute_variance ), n_scanline_plane_adapter<false, decltype(compute_variance)>(), dispatch_scan_processing, op::n_to_one ) );
}

} // image



