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

#ifdef __SSE__
# if defined(LINUX) || defined(__linux__)
#  include <x86intrin.h>
# else
#  include <xmmintrin.h>
#  include <immintrin.h>
# endif
#endif

#include <image/threading.h>

////////////////////////////////////////

namespace
{
using namespace image;

////////////////////////////////////////

static void sat_cols1( size_t, int s, int e, accum_buf &dest, const plane &p )
{
	int dS = dest.stride();
	int pS = p.stride();
	int h = dest.height();

	int x = s;
	while ( x < e )
	{
		if ( x % 4 == 0 && ( x + 4 ) < e )
		{
			__m128d sumL = _mm_setzero_pd();
			__m128d sumH = _mm_setzero_pd();
			double *out = dest.data() + x;
			const float *col = p.cdata() + x;
			for ( int c = 0; c < h; ++c, out += dS, col += pS )
			{
				__m128 inV = _mm_load_ps( col );
				__m128d loV = _mm_cvtps_pd( inV );
				__m128d hiV = _mm_cvtps_pd( _mm_movehl_ps( inV, inV ) );
				sumL = _mm_add_pd( sumL, loV );
				sumH = _mm_add_pd( sumH, hiV );
				_mm_store_pd( out, sumL );
				_mm_store_pd( out + 2, sumH );
			}
			x += 4;
		}
		else
		{
			double sum = 0.0;
			double *out = dest.data() + x;
			const float *col = p.cdata() + x;
			for ( int c = 0; c < h; ++c, out += dS, col += pS )
			{
				sum += static_cast<double>( *col );
				*out = sum;
			}
			++x;
		}
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
	const __m128d zed = _mm_setzero_pd();
	for ( int y = s; y < e; ++y )
	{
		double *out = dest.line( y );
		__m128d sum = zed;
		for ( int x = 0; x < w; x += 2 )
		{
			__m128d v = _mm_load_pd( out );
			sum = _mm_add_pd( sum, _mm_movedup_pd( v ) );
			sum = _mm_add_pd( sum, _mm_move_sd( v, zed ) );
			_mm_store_pd( out, sum );

			sum = _mm_add_pd( sum, _mm_shuffle_pd( v, zed, 1 ) );

			out += 2;
		}
	}
}

}

////////////////////////////////////////

namespace image
{

namespace sse3
{

////////////////////////////////////////

accum_buf
compute_SAT( const plane &p, int power )
{
	accum_buf dest( p.width(), p.height() );
	switch ( power )
	{
		case 1:
			threading::get().dispatch( std::bind( sat_cols1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ), std::cref( p ) ), 0, p.width() );
			break;
		default:
			threading::get().dispatch( std::bind( sat_cols, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ), std::cref( p ), power ), 0, p.width() );
			break;
	}

	threading::get().dispatch( std::bind( sat_rows, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( dest ) ), 0, dest.height() );

	return dest;
}

} // sse3

}


