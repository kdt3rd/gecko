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

#include "patch_match.h"
#include "plane_ops.h"
#include <random>

////////////////////////////////////////

namespace
{
using namespace image;

struct PatchMatchSSD
{
	static inline float compute( const plane &a, const plane &b, int px, int py, int radius )
	{
		float ret = 0.F;
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += 1.F;
				}
				else
				{
					float d = a.get( sx, sy ) - b.get( sx, sy );
					ret += d * d;
				}
			}
		}
		return ret;
	}

	static inline float compute( const image_buf &a, const image_buf &b, int px, int py, int radius )
	{
		float ret = 0.F;
		int numC = a.planes();
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( numC * (2*radius + 1) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( int c = 0; c != numC; ++c )
					{
						float d = a[c].get( sx, sy ) - b[c].get( sx, sy );
						ret += d * d;
					}
				}
			}
		}
		return ret;
	}
};

struct PatchMatchSSDGrad
{
	static inline float compute( const plane &a, const plane &b, int px, int py, int radius )
	{
		float ret = 0.F;
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += 1.F;
				}
				else
				{
					float d = a.get( sx, sy ) - b.get( sx, sy );
					float gxa = a.get_zero( sx + 1, sy ) - a.get_zero( sx - 1, sy );
					float gxb = b.get_zero( sx + 1, sy ) - b.get_zero( sx - 1, sy );
					float gya = a.get_zero( sx, sy + 1 ) - a.get_zero( sx, sy - 1 );
					float gyb = b.get_zero( sx, sy + 1 ) - b.get_zero( sx, sy - 1 );

					gxa -= gxb;
					gya -= gyb;
					ret += d * d + gxa * gxa + gya * gya;
				}
			}
		}
		return ret;
	}

	static inline float compute( const image_buf &a, const image_buf &b, int px, int py, int radius )
	{
		float ret = 0.F;
		int numC = a.planes();
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( numC * ( 2*radius + 1 ) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( int c = 0; c != numC; ++c )
					{
						float d = a[c].get( sx, sy ) - b[c].get( sx, sy );
						float gxa = a[c].get_zero( sx + 1, sy ) - a[c].get_zero( sx - 1, sy );
						float gxb = b[c].get_zero( sx + 1, sy ) - b[c].get_zero( sx - 1, sy );
						float gya = a[c].get_zero( sx, sy + 1 ) - a[c].get_zero( sx, sy - 1 );
						float gyb = b[c].get_zero( sx, sy + 1 ) - b[c].get_zero( sx, sy - 1 );

						gxa -= gxb;
						gya -= gyb;
						ret += d * d + gxa * gxa + gya * gya;
					}
				}
			}
		}
		return ret;
	}
};

struct PatchMatchGrad
{
	static inline float compute( const plane &a, const plane &b, int px, int py, int radius )
	{
		float ret = 0.F;
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += 1.F;
				}
				else
				{
					float gxa = a.get_zero( sx + 1, sy ) - a.get_zero( sx - 1, sy );
					float gxb = b.get_zero( sx + 1, sy ) - b.get_zero( sx - 1, sy );
					float gya = a.get_zero( sx, sy + 1 ) - a.get_zero( sx, sy - 1 );
					float gyb = b.get_zero( sx, sy + 1 ) - b.get_zero( sx, sy - 1 );

					gxa -= gxb;
					gya -= gyb;
					ret += gxa * gxa + gya * gya;
				}
			}
		}
		return ret;
	}

	static inline float compute( const image_buf &a, const image_buf &b, int px, int py, int radius )
	{
		float ret = 0.F;
		int numC = a.planes();
		for ( int y = -radius; y <= radius; ++y )
		{
			int sy = py + y;
			if ( sy < 0 || sy >= a.height() )
			{
				ret += static_cast<float>( numC * ( 2*radius + 1 ) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sx = px + x;
				if ( sx < 0 || sx >= a.width() )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( int c = 0; c != numC; ++c )
					{
						float gxa = a[c].get_zero( sx + 1, sy ) - a[c].get_zero( sx - 1, sy );
						float gxb = b[c].get_zero( sx + 1, sy ) - b[c].get_zero( sx - 1, sy );
						float gya = a[c].get_zero( sx, sy + 1 ) - a[c].get_zero( sx, sy - 1 );
						float gyb = b[c].get_zero( sx, sy + 1 ) - b[c].get_zero( sx, sy - 1 );

						gxa -= gxb;
						gya -= gyb;
						ret += gxa * gxa + gya * gya;
					}
				}
			}
		}
		return ret;
	}
};

template <typename BufType, typename DistFunc>
inline int64_t
minimizePatch( plane &u, plane &v, plane &d, const BufType &a, const BufType &b, int x, int y, std::mt19937 &gen, int radius, int dir )
{
	float curDist = d.get( x, y );

	if ( curDist <= 0.F )
		return 0;

	bool change = false;
	int maxX = a.width() - 1;
	int maxY = a.height() - 1;

	// check left/right
	int horiz = x - dir;
	int curDX = static_cast<int>( u.get( x, y ) );
	int curDY = static_cast<int>( v.get( x, y ) );
	if ( horiz >= 0 && horiz < u.width() )
	{
		int altDX = static_cast<int>( u.get( horiz, y ) ) + dir;
		int altDY = static_cast<int>( v.get( horiz, y ) );
		float hDist = DistFunc::compute( a, b, altDX, altDY, radius );
		if ( hDist < curDist )
		{
			curDX = altDX;
			curDY = altDY;
			change = true;
			curDist = hDist;
		}
	}
	// check up/down
	int vert = y - dir;
	if ( vert >= 0 && vert < u.height() )
	{
		int altDX = static_cast<int>( u.get( x, vert ) );
		int altDY = static_cast<int>( v.get( x, vert ) ) + dir;
		float hDist = DistFunc::compute( a, b, altDX, altDY, radius );
		if ( hDist < curDist )
		{
			curDX = altDX;
			curDY = altDY;
			change = true;
			curDist = hDist;
		}
	}

	// perform random search
	int srch = std::max( a.width(), a.height() );
	int startDX = curDX;
	int startDY = curDY;
	while ( srch > 0 )
	{
		int r = static_cast<int>( gen() ) % (2*srch) - srch;
		int altDX = startDX + r;
		r = static_cast<int>( gen() ) % (2*srch) - srch;
		int altDY = startDY + r;
		altDX = std::max( int(0), std::min( maxX, altDX ) );
		altDY = std::max( int(0), std::min( maxY, altDY ) );

		float hDist = DistFunc::compute( a, b, altDX, altDY, radius );
		if ( hDist < curDist )
		{
			curDX = altDX;
			curDY = altDY;
			change = true;
			curDist = hDist;
		}

		srch /= 2;
	}

	if ( change )
	{
		u.get( x, y ) = static_cast<float>( curDX );
		v.get( x, y ) = static_cast<float>( curDY );
		d.get( x, y ) = curDist;
		return 1;
	}
	return 0;
}

template <typename BufType, typename DistFunc>
inline void
matchPassInit( plane &u, plane &v, plane &d, const BufType &a, const BufType &b, int radius )
{
	for ( int y = 0; y < u.height(); ++y )
	{
		for ( int x = 0; x < u.width(); ++x )
		{
			int curDX = static_cast<int>( u.get( x, y ) );
			int curDY = static_cast<int>( v.get( x, y ) );
			d.get( x, y ) = DistFunc::compute( a, b, curDX, curDY, radius );
			if ( x == y )
				std::cout << x << ", " << y << " curDX " << curDX << " curDY " << curDY << ": " << d.get( x, y ) << std::endl;
		}
	}
}

template <typename BufType, typename DistFunc>
inline int64_t
matchPass( plane &u, plane &v, plane &d, const BufType &a, const BufType &b, std::mt19937 &gen, int radius, int dir, int iter )
{
	int64_t changeCount = 0;
	if ( dir == 1 )
	{
		for ( int y = u.height() - 1; y >= 0; --y )
		{
			std::cout << "\rpatch_match iter " << iter << " minimize line " << y << std::flush;
			for ( int x = u.width() - 1; x >= 0; --x )
				changeCount += minimizePatch<BufType,DistFunc>( u, v, d, a, b, x, y, gen, radius, dir );
		}
	}
	else
	{
		for ( int y = 0; y < u.height(); ++y )
		{
			std::cout << "\rpatch_match iter " << iter << " minimize line " << y << std::flush;
			for ( int x = 0; x < u.width(); ++x )
				changeCount += minimizePatch<BufType,DistFunc>( u, v, d, a, b, x, y, gen, radius, dir );
		}
	}
	return changeCount;
}

template<typename BufType>
inline void
runMatch( plane &u, plane &v, plane &d, const BufType &a, const BufType &b, uint32_t srchseed, int radius, int style, int iters )
{
	std::mt19937 gen( srchseed );
	int64_t changeCount;

	switch ( static_cast<patch_style>( style ) )
	{
		case patch_style::SSD:
			matchPassInit<BufType,PatchMatchSSD>( u, v, d, a, b, radius );
			for ( int p = 0; p < iters; ++p )
			{
				std::cout << "\npatch_match iter " << p << std::endl;
				// if changes don't change, finished
				changeCount = matchPass<BufType,PatchMatchSSD>( u, v, d, a, b, gen, radius, -1, p );
				changeCount += matchPass<BufType,PatchMatchSSD>( u, v, d, a, b, gen, radius, 1, p );
				if ( changeCount == 0 )
					break;
			}
			break;
		case patch_style::SSD_GRAD:
			matchPassInit<BufType,PatchMatchSSDGrad>( u, v, d, a, b, radius );
			for ( int p = 0; p < iters; ++p )
			{
				std::cout << "\npatch_match iter " << p << std::endl;
				changeCount = matchPass<BufType,PatchMatchSSDGrad>( u, v, d, a, b, gen, radius, -1, p );
				changeCount += matchPass<BufType,PatchMatchSSDGrad>( u, v, d, a, b, gen, radius, 1, p );
				if ( changeCount == 0 )
					break;
			}
			break;
		case patch_style::GRAD:
			matchPassInit<BufType,PatchMatchGrad>( u, v, d, a, b, radius );
			for ( int p = 0; p < iters; ++p )
			{
				std::cout << "\npatch_match iter " << p << std::endl;
				changeCount = matchPass<BufType,PatchMatchGrad>( u, v, d, a, b, gen, radius, -1, p );
				changeCount += matchPass<BufType,PatchMatchGrad>( u, v, d, a, b, gen, radius, 1, p );
				if ( changeCount == 0 )
					break;
			}
			break;
	}
}

static vector_field pmPlane( const plane &a, const plane &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	plane dist = create_plane( a.width(), a.height(), std::numeric_limits<plane::value_type>::max() );
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );
//	uint32_t seedV = static_cast<uint32_t>( frameA + (frameA - frameB) - 1 );
//	plane u = create_random_plane( a.width(), a.height(), seedU, 0.F, static_cast<float>( a.width() ) );
//	plane v = create_random_plane( a.width(), a.height(), seedV, 0.F, static_cast<float>( a.height() ) );
	plane u = create_iotaX_plane( a.width(), a.height() );
	plane v = create_iotaY_plane( a.width(), a.height() );
	runMatch( u, v, dist, a, b, seedU, radius, style, iters );

	return vector_field::create( std::move( u ), std::move( v ) );
}

static vector_field pmImage( const image_buf &a, const image_buf &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	plane dist = create_plane( a.width(), a.height(), std::numeric_limits<plane::value_type>::max() );
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );
	uint32_t seedV = static_cast<uint32_t>( frameA + (frameA - frameB) - 1 );
	plane u = create_random_plane( a.width(), a.height(), seedU, 0.F, static_cast<float>( a.width() ) );
	plane v = create_random_plane( a.width(), a.height(), seedV, 0.F, static_cast<float>( a.height() ) );
	runMatch( u, v, dist, a, b, seedU, radius, style, iters );

	return vector_field::create( std::move( u ), std::move( v ) );
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

vector_field patch_match( const plane &a, const plane &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters )
{
	engine::dimensions d;
	precondition( a.dims() == b.dims(), "patch_match must have a & b of same size, received {0}x{1} and {3}x{4}", a.width(), a.height(), b.width(), b.height() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;

	return vector_field( "p.patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////

vector_field patch_match( const image_buf &a, const image_buf &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters )
{
	engine::dimensions d;
	precondition( !a.empty() && a.dims() == b.dims(), "patch_match must have a & b of same size, received {0}x{1}x{2} and {3}x{4}x{5}", a.width(), a.height(), a.planes(), b.width(), b.height(), b.planes() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;
	return vector_field( "i.patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////

void
add_patchmatch( engine::registry &r )
{
	using namespace engine;
	r.add( op( "p.patch_match", pmPlane, op::threaded ) );
	r.add( op( "i.patch_match", pmImage, op::threaded ) );
	
}

////////////////////////////////////////

} // namespace image

