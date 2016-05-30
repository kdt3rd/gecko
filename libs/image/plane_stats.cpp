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

////////////////////////////////////////

namespace
{

static void sum_thread( size_t tIdx, int s, int e, const image::plane &p, std::vector<double> &vals )
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

static double sum_plane( const image::plane &p )
{
	std::vector<double> vals;
	vals.resize( image::threading::get().size(), 0.0 );

	image::threading::get().dispatch( std::bind( sum_thread, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::cref( p ), std::ref( vals ) ), p );

	double r = 0.0;
	for ( double d: vals )
		r += d;
	return r;
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

void
add_plane_stats( engine::registry &r )
{
	using namespace engine;

	r.add( op( "sum_plane", sum_plane, op::threaded ) );
}

} // image



