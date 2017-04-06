//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "patch_match.h"
#include "plane_ops.h"
#include "media_io.h"
#include "threading.h"
#include <random>
#include <iomanip>

////////////////////////////////////////

namespace
{
using namespace image;

struct PatchMatchSSD
{
	static inline float compute( const const_plane_buffer &a, const const_plane_buffer &b, const const_plane_buffer &, const const_plane_buffer &, const const_plane_buffer &, const const_plane_buffer &, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= a.height() )
				continue;
			if ( sby < 0 || sby >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= a.width() )
					continue;
				if ( sbx < 0 || sbx >= a.width() )
				{
						ret += 1.F;
				}
				else
				{
					float d = a.get( sax, say ) - b.get( sbx, sby );
					ret += d * d;
				}
			}
		}
		return ret;
	}

	static inline float compute( const std::vector<const_plane_buffer> &a, const std::vector<const_plane_buffer> &b, const std::vector<const_plane_buffer> &, const std::vector<const_plane_buffer> &, const std::vector<const_plane_buffer> &, const std::vector<const_plane_buffer> &, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		size_t numC = a.size();
		int w = a[0].width();
		int h = a[0].height();
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= h )
				continue;
			if ( sby < 0 || sby >= h )
			{
				ret += static_cast<float>( static_cast<int>( numC ) * (2*radius + 1) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= w )
					continue;
				if ( sbx < 0 || sbx >= w )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( size_t c = 0; c != numC; ++c )
					{
						float d = a[c].get( sax, say ) - b[c].get( sbx, sby );
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
	static inline float compute( const const_plane_buffer &a, const const_plane_buffer &b, const const_plane_buffer &adx, const const_plane_buffer &ady, const const_plane_buffer &bdx, const const_plane_buffer &bdy, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= a.height() )
				continue;
			if ( sby < 0 || sby >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= a.width() )
					continue;
				if ( sbx < 0 || sbx >= a.width() )
				{
					ret += 1.F;
				}
				else
				{
					float d = a.get( sax, say ) - b.get( sbx, sby );
//					float gxa = get_zero( a, sax + 1, say ) - get_zero( a, sax - 1, say );
//					float gxb = get_zero( b, sbx + 1, sby ) - get_zero( b, sbx - 1, sby );
//					float gya = get_zero( a, sax, say + 1 ) - get_zero( a, sax, say - 1 );
//					float gyb = get_zero( b, sbx, sby + 1 ) - get_zero( b, sbx, sby - 1 );
//					gxa -= gxb;
//					gya -= gyb;
//					ret += d * d + gxa * gxa + gya * gya;

					float gx = adx.get( sax, say ) - bdx.get( sbx, sby );
					float gy = ady.get( sax, say ) - bdy.get( sbx, sby );
					ret += d * d + gx * gx + gy * gy;
				}
			}
		}
		return ret;
	}

	static inline float compute( const std::vector<const_plane_buffer> &a, const std::vector<const_plane_buffer> &b, const std::vector<const_plane_buffer> &adx, const std::vector<const_plane_buffer> &ady, const std::vector<const_plane_buffer> &bdx, const std::vector<const_plane_buffer> &bdy, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		size_t numC = a.size();
		int w = a[0].width();
		int h = a[0].height();
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= h )
				continue;
			if ( sby < 0 || sby >= h )
			{
				ret += static_cast<float>( static_cast<int>( numC ) * ( 2*radius + 1 ) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= w )
					continue;
				if ( sbx < 0 || sbx >= w )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( size_t c = 0; c != numC; ++c )
					{
						float d = a[c].get( sax, say ) - b[c].get( sbx, sby );
//						float gxa = get_zero( a[c], sax + 1, say ) - get_zero( a[c], sax - 1, say );
//						float gxb = get_zero( b[c], sbx + 1, sby ) - get_zero( b[c], sbx - 1, sby );
//						float gya = get_zero( a[c], sax, say + 1 ) - get_zero( a[c], sax, say - 1 );
//						float gyb = get_zero( b[c], sbx, sby + 1 ) - get_zero( b[c], sbx, sby - 1 );
//						gxa -= gxb;
//						gya -= gyb;
//						ret += d * d + gxa * gxa + gya * gya;

						float gx = adx[c].get( sax, say ) - bdx[c].get( sbx, sby );
						float gy = ady[c].get( sax, say ) - bdy[c].get( sbx, sby );
						ret += d * d + gx * gx + gy * gy;
					}
				}
			}
		}
		return ret;
	}
};


struct PatchMatchSSDGradDist
{
	static inline float compute( const const_plane_buffer &a, const const_plane_buffer &b, const const_plane_buffer &adx, const const_plane_buffer &ady, const const_plane_buffer &bdx, const const_plane_buffer &bdy, int ax, int ay, int px, int py, int radius )
	{
		float distX = static_cast<float>( ax - px ) / static_cast<float>( a.width() );
		float distY = static_cast<float>( ay - py ) / static_cast<float>( a.height() );
		float ret = distX * distX + distY * distY;
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= a.height() )
				continue;
			if ( sby < 0 || sby >= a.height() )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= a.width() )
					continue;
				if ( sbx < 0 || sbx >= a.width() )
				{
					ret += 1.F;
				}
				else
				{
					float d = a.get( sax, say ) - b.get( sbx, sby );
					float gx = adx.get( sax, say ) - bdx.get( sbx, sby );
					float gy = ady.get( sax, say ) - bdy.get( sbx, sby );
					ret += d * d + gx * gx + gy * gy;
				}
			}
		}
		return ret;
	}

	static inline float compute( const std::vector<const_plane_buffer> &a, const std::vector<const_plane_buffer> &b, const std::vector<const_plane_buffer> &adx, const std::vector<const_plane_buffer> &ady, const std::vector<const_plane_buffer> &bdx, const std::vector<const_plane_buffer> &bdy, int ax, int ay, int px, int py, int radius )
	{
		int w = a[0].width();
		int h = a[0].height();
		float distX = static_cast<float>( ax - px ) / static_cast<float>( w );
		float distY = static_cast<float>( ay - py ) / static_cast<float>( h );
		float ret = distX * distX + distY * distY;
		size_t numC = a.size();
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= h )
				continue;
			if ( sby < 0 || sby >= h )
			{
				ret += static_cast<float>( static_cast<int>( numC ) * ( 2*radius + 1 ) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= w )
					continue;
				if ( sbx < 0 || sbx >= w )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( size_t c = 0; c != numC; ++c )
					{
						float d = a[c].get( sax, say ) - b[c].get( sbx, sby );
						float gx = adx[c].get( sax, say ) - bdx[c].get( sbx, sby );
						float gy = ady[c].get( sax, say ) - bdy[c].get( sbx, sby );
						ret += d * d + gx * gx + gy * gy;
					}
				}
			}
		}
		return ret;
	}
};

struct PatchMatchGrad
{
	static inline float compute( const const_plane_buffer &, const const_plane_buffer &, const const_plane_buffer &adx, const const_plane_buffer &ady, const const_plane_buffer &bdx, const const_plane_buffer &bdy, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		int w = adx.width();
		int h = adx.height();
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= h )
				continue;
			if ( sby < 0 || sby >= h )
			{
				ret += static_cast<float>( 2*radius + 1 );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= w )
					continue;
				if ( sbx < 0 || sbx >= w )
				{
					ret += 1.F;
				}
				else
				{
					float gx = adx.get( sax, say ) - bdx.get( sbx, sby );
					float gy = ady.get( sax, say ) - bdy.get( sbx, sby );
					ret += gx * gx + gy * gy;
				}
			}
		}
		return ret;
	}

	static inline float compute( const std::vector<const_plane_buffer> &, const std::vector<const_plane_buffer> &, const std::vector<const_plane_buffer> &adx, const std::vector<const_plane_buffer> &ady, const std::vector<const_plane_buffer> &bdx, const std::vector<const_plane_buffer> &bdy, int ax, int ay, int px, int py, int radius )
	{
		float ret = 0.F;
		size_t numC = adx.size();
		int w = adx[0].width();
		int h = adx[0].height();
		for ( int y = -radius; y <= radius; ++y )
		{
			int say = ay + y;
			int sby = py + y;
			if ( say < 0 || say >= h )
				continue;
			if ( sby < 0 || sby >= h )
			{
				ret += static_cast<float>( static_cast<int>( numC ) * ( 2*radius + 1 ) );
				continue;
			}

			for ( int x = -radius; x <= radius; ++x )
			{
				int sax = ax + x;
				int sbx = px + x;
				if ( sax < 0 || sax >= w )
					continue;
				if ( sbx < 0 || sbx >= w )
				{
					ret += static_cast<float>( numC );
				}
				else
				{
					for ( size_t c = 0; c != numC; ++c )
					{
						float gx = adx[c].get( sax, say ) - bdx[c].get( sbx, sby );
						float gy = ady[c].get( sax, say ) - bdy[c].get( sbx, sby );
						ret += gx * gx + gy * gy;
					}
				}
			}
		}
		return ret;
	}
};

////////////////////////////////////////

template <typename BufType, typename DistFunc>
static void
matchPassInitThread( size_t, int s, int e, plane_buffer &d, const plane_buffer &u, const plane_buffer &v, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius )
{
	int w = d.width();
	for ( int y = s; y < e; ++y )
	{
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		float *dLine = d.line( y );
		for ( int x = 0; x < w; ++x )
		{
			int curDX = static_cast<int>( uLine[x] );
			int curDY = static_cast<int>( vLine[x] );
			dLine[x] = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, curDX, curDY, radius );
		}
	}
}

template <typename BufType, typename DistFunc>
inline void
matchPassInit( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius )
{
	std::cout << "patch_match init: " << std::flush;
	threading::get().dispatch( std::bind( matchPassInitThread<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( d ), std::cref( u ), std::cref( v ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius ), 0, u.height() );
	std::cout << "finished" << std::endl;
}

////////////////////////////////////////

template <typename BufType, typename DistFunc>
static void
matchPassHorizThreadForward1( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, float eps )
{
	int w = d.width();
	size_t changeCount = 0;
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );

		for ( int x = 1; x < w; ++x )
		{
			float &curDist = dLine[x];
			if ( curDist <= eps )
				continue;
			int altDX = static_cast<int>( uLine[x-1] ) + 1;
			int altDY = static_cast<int>( vLine[x-1] );

			float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				uLine[x] = static_cast<float>( altDX );
				vLine[x] = static_cast<float>( altDY );
				curDist = hDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassHorizThreadForwardN( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, int maxSkip, float eps )
{
	int w = d.width();
	size_t changeCount = 0;
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );

		for ( int x = 1; x < w; ++x )
		{
			float curDist = dLine[x];
			if ( curDist <= eps )
				continue;

			int curDX = uLine[x];
			int curDY = vLine[x];
			bool changed = false;

			int skip = maxSkip;
			while ( skip > 0 )
			{
				if ( skip <= x )
				{
					int altDX = static_cast<int>( uLine[x-skip] ) + skip;
					int altDY = static_cast<int>( vLine[x-skip] );

					float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );

					if ( hDist < curDist )
					{
						curDX = altDX;
						curDY = altDY;
						curDist = hDist;
						changed = true;
					}
				}
				skip /= 2;
			}
			if ( changed )
			{
				uLine[x] = static_cast<float>( curDX );
				vLine[x] = static_cast<float>( curDY );
				dLine[x] = curDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassHorizThreadBackward1( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, float eps )
{
	int w = d.width();
	size_t changeCount = 0;
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );

		for ( int x = w - 2; x >= 0; --x )
		{
			float &curDist = dLine[x];
			if ( curDist <= eps )
				continue;
			int altDX = static_cast<int>( uLine[x+1] ) - 1;
			int altDY = static_cast<int>( vLine[x+1] );

			float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				uLine[x] = static_cast<float>( altDX );
				vLine[x] = static_cast<float>( altDY );
				curDist = hDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassHorizThreadBackwardN( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, int maxSkip, float eps )
{
	int w = d.width();
	size_t changeCount = 0;
	for ( int y = s; y < e; ++y )
	{
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );

		for ( int x = w - 2; x >= 0; --x )
		{
			float curDist = dLine[x];
			if ( curDist <= eps )
				continue;
			int curDX = uLine[x];
			int curDY = vLine[x];
			bool changed = false;

			int skip = maxSkip;
			while ( skip > 0 )
			{
				if ( (x + skip) < w )
				{
					int altDX = static_cast<int>( uLine[x+skip] ) - skip;
					int altDY = static_cast<int>( vLine[x+skip] );

					float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
					if ( hDist < curDist )
					{
						curDX = altDX;
						curDY = altDY;
						curDist = hDist;
						changed = true;
					}
				}
				skip /= 2;
			}
			if ( changed )
			{
				uLine[x] = static_cast<float>( curDX );
				vLine[x] = static_cast<float>( curDY );
				dLine[x] = curDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassVertThreadDown1( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, float eps )
{
	int h = d.height();
	size_t changeCount = 0;
	for ( int x = s; x < e; ++x )
	{
		for ( int y = 1; y < h; ++y )
		{
			float &curDist = d.get( x, y );
			if ( curDist <= eps )
				continue;

			int altDX = static_cast<int>( u.get( x, y - 1 ) );
			int altDY = static_cast<int>( v.get( x, y - 1 ) ) + 1;

			float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				u.get( x, y ) = static_cast<float>( altDX );
				v.get( x, y ) = static_cast<float>( altDY );
				curDist = hDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassVertThreadDownN( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, int maxSkip, float eps )
{
	int h = d.height();
	size_t changeCount = 0;
	for ( int x = s; x < e; ++x )
	{
		for ( int y = 1; y < h; ++y )
		{
			float &curDist = d.get( x, y );
			if ( curDist <= eps )
				continue;

			bool changed = false;
			int skip = maxSkip;
			while ( skip > 0 )
			{
				if ( y >= skip )
				{
					int altDX = static_cast<int>( u.get( x, y - skip ) );
					int altDY = static_cast<int>( v.get( x, y - skip ) ) + skip;

					float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
					if ( hDist < curDist )
					{
						u.get( x, y ) = static_cast<float>( altDX );
						v.get( x, y ) = static_cast<float>( altDY );
						curDist = hDist;
						changed = true;
					}
				}
				skip /= 2;
			}
			if ( changed )
				++changeCount;
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassVertThreadUp1( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, float eps )
{
	int h = d.height();
	size_t changeCount = 0;
	for ( int x = s; x < e; ++x )
	{
		for ( int y = h - 2; y >= 0; --y )
		{
			float &curDist = d.get( x, y );
			if ( curDist <= eps )
				continue;
			int altDX = static_cast<int>( u.get( x, y + 1 ) );
			int altDY = static_cast<int>( v.get( x, y + 1 ) ) - 1;

			float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				u.get( x, y ) = static_cast<float>( altDX );
				v.get( x, y ) = static_cast<float>( altDY );
				curDist = hDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassVertThreadUpN( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, int maxSkip, float eps )
{
	int h = d.height();
	size_t changeCount = 0;
	for ( int x = s; x < e; ++x )
	{
		for ( int y = h - 2; y >= 0; --y )
		{
			float &curDist = d.get( x, y );
			if ( curDist <= eps )
				continue;

			bool changed = false;
			int skip = maxSkip;
			while ( skip > 0 )
			{
				if ( (skip + y) < h )
				{
					int altDX = static_cast<int>( u.get( x, y + skip ) );
					int altDY = static_cast<int>( v.get( x, y + skip ) ) - skip;

					float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
					if ( hDist < curDist )
					{
						u.get( x, y ) = static_cast<float>( altDX );
						v.get( x, y ) = static_cast<float>( altDY );
						curDist = hDist;
						changed = true;
					}
				}
				skip /= 2;
			}
			if ( changed )
				++changeCount;
		}
	}
	changeCounts[tIdx] = changeCount;
}


template <typename BufType, typename DistFunc>
static void
matchPassDiagonal( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, std::vector<size_t> &changeCounts, int red, float eps )
{
	int w = d.width();
	int h = d.height();
	size_t changeCount = 0;
	for ( int hy = s; hy < e; ++hy )
	{
		int y = hy * 2 + red;
		if ( y < 1 || ( y >= ( h - 1 ) ) )
			continue;

		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );
		const float *prevULine = u.line( y - 1 );
		const float *prevVLine = v.line( y - 1 );
		const float *nextULine = u.line( y + 1 );
		const float *nextVLine = v.line( y + 1 );
		for ( int x = 1; x < (w - 1); ++x )
		{
			int curDX = uLine[x];
			int curDY = vLine[x];
			float curDist = dLine[x];
			if ( curDist <= eps )
				continue;
			
			bool changed = false;

			int altDX = static_cast<int>( prevULine[x-1] ) + 1;
			int altDY = static_cast<int>( prevVLine[x-1] ) + 1;
			float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				curDX = altDX;
				curDY = altDY;
				curDist = hDist;
				changed = true;
			}

			altDX = static_cast<int>( prevULine[x+1] ) - 1;
			altDY = static_cast<int>( prevVLine[x+1] ) + 1;
			hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				curDX = altDX;
				curDY = altDY;
				curDist = hDist;
				changed = true;
			}

			altDX = static_cast<int>( nextULine[x-1] ) + 1;
			altDY = static_cast<int>( nextVLine[x-1] ) - 1;
			hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				curDX = altDX;
				curDY = altDY;
				curDist = hDist;
				changed = true;
			}

			altDX = static_cast<int>( nextULine[x+1] ) - 1;
			altDY = static_cast<int>( nextVLine[x+1] ) - 1;
			hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
			if ( hDist < curDist )
			{
				curDX = altDX;
				curDY = altDY;
				curDist = hDist;
				changed = true;
			}

			if ( changed )
			{
				uLine[x] = static_cast<float>( curDX );
				vLine[x] = static_cast<float>( curDY );
				dLine[x] = curDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

template <typename BufType, typename DistFunc>
static void
matchPassRandomThread( size_t tIdx, int s, int e, plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int radius, const std::vector<uint32_t> &seeds, std::vector<size_t> &changeCounts, float eps )
{
	int w = d.width();
	int h = d.width();
	int srch = std::max( w, h );
	int maxX = w - 1;
	int maxY = h - 1;
	size_t changeCount = 0;
	for ( int y = s; y < e; ++y )
	{
		std::mt19937 rndg( seeds[y] );
		float *uLine = u.line( y );
		float *vLine = v.line( y );
		float *dLine = d.line( y );
		for ( int x = 0; x < w; ++x )
		{
			int curDX = static_cast<int>( uLine[x] );
			int curDY = static_cast<int>( vLine[x] );
			float curDist = dLine[x];
			if ( curDist <= eps )
				continue;

			// perform random search
			int curSrch = srch;
			int startDX = curDX;
			int startDY = curDY;
			bool change = false;
			while ( curSrch > 0 )
			{
				int r = static_cast<int>( rndg() ) % (2*curSrch) - curSrch;
				int altDX = startDX + r;
				r = static_cast<int>( rndg() ) % (2*curSrch) - curSrch;
				int altDY = startDY + r;
				altDX = std::max( int(0), std::min( maxX, altDX ) );
				altDY = std::max( int(0), std::min( maxY, altDY ) );

				float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
				if ( hDist < curDist )
				{
					curDX = altDX;
					curDY = altDY;
					change = true;
					curDist = hDist;
				}

				curSrch /= 2;
			}
			if ( change )
			{
				u.get( x, y ) = static_cast<float>( curDX );
				v.get( x, y ) = static_cast<float>( curDY );
				d.get( x, y ) = curDist;
				++changeCount;
			}
		}
	}
	changeCounts[tIdx] = changeCount;
}

static inline size_t
getCount( std::vector<size_t> &counts )
{
	size_t ret = 0;
	for ( size_t &a: counts )
	{
		ret += a;
		a = 0;
	}
	return ret;
}

template <typename BufType, typename DistFunc>
static bool matchPass2( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, std::mt19937 &gen, int radius, int iter, float eps )
{
	std::vector<size_t> counts( threading::get().size(), size_t(0) );
	size_t nChange;
	size_t totChange = 0;
	// do one round of jump flood on second iteration
	int maxSkipX = ( iter == 1 ) ? std::max( int(1), u.width() / (iter * iter + 1) ) : 1;
	int maxSkipY = ( iter == 1 ) ? std::max( int(1), u.height() / (iter * iter + 1) ) : 1;

	std::cout << "   iter " << iter
			  << " skip: " << maxSkipX << ' ' << maxSkipY
			  << " l->r: " << std::flush;
	if ( maxSkipX == 1 )
		threading::get().dispatch( std::bind( matchPassHorizThreadForward1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.height() );
	else
		threading::get().dispatch( std::bind( matchPassHorizThreadForwardN<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), maxSkipX, eps ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << " t->b: " << std::flush;
	totChange += nChange;

	if ( maxSkipY == 1 )
		threading::get().dispatch( std::bind( matchPassVertThreadDown1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.width() );
	else
		threading::get().dispatch( std::bind( matchPassVertThreadDownN<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), maxSkipY, eps ), 0, u.width() );
	nChange = getCount( counts );
	std::cout << nChange << " random: " << std::flush;
	totChange += nChange;

	size_t nSeeds = static_cast<size_t>( u.height() );
	std::vector<uint32_t> seeds( nSeeds, 0 );
	for ( size_t y = 0; y < nSeeds; ++y )
		seeds[y] = gen();

	threading::get().dispatch( std::bind( matchPassRandomThread<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::cref( seeds ), std::ref( counts ), eps ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << " r->l: " << std::flush;
	totChange += nChange;

	if ( maxSkipX == 1 )
		threading::get().dispatch( std::bind( matchPassHorizThreadBackward1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.height() );
	else
		threading::get().dispatch( std::bind( matchPassHorizThreadBackwardN<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), maxSkipX, eps ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << " b->t: " << std::flush;
	totChange += nChange;

	if ( maxSkipY == 1 )
		threading::get().dispatch( std::bind( matchPassVertThreadUp1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.width() );
	else
		threading::get().dispatch( std::bind( matchPassVertThreadUpN<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), maxSkipY, eps ), 0, u.width() );
	nChange = getCount( counts );
	std::cout << nChange << " EVEN: " << std::flush;
	totChange += nChange;

	threading::get().dispatch( std::bind( matchPassDiagonal<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), 0, eps ), 0, u.height() / 2 );
	nChange = getCount( counts );
	std::cout << nChange << " ODD: " << std::flush;
	totChange += nChange;

	threading::get().dispatch( std::bind( matchPassDiagonal<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), 1, eps ), 0, u.height() / 2 );
	nChange = getCount( counts );
	std::cout << nChange << std::endl;
	totChange += nChange;

	return totChange == 0;
}

////////////////////////////////////////

template <typename BufType, typename DistFunc>
inline int64_t
minimizePatch( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, int x, int y, std::mt19937 &gen, int radius, int dir, float eps )
{
	float curDist = d.get( x, y );

	if ( curDist <= eps )
		return 0;

	bool change = false;
	int maxX = u.width() - 1;
	int maxY = u.height() - 1;

	// check left/right
	int horiz = x - dir;
	int curDX = static_cast<int>( u.get( x, y ) );
	int curDY = static_cast<int>( v.get( x, y ) );
	if ( horiz >= 0 && horiz < u.width() )
	{
		int altDX = static_cast<int>( u.get( horiz, y ) ) + dir;
		int altDY = static_cast<int>( v.get( horiz, y ) );
		float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
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
		float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
		if ( hDist < curDist )
		{
			curDX = altDX;
			curDY = altDY;
			change = true;
			curDist = hDist;
		}
	}

	// perform random search
	int srch = std::max( u.width(), u.height() );
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

		float hDist = DistFunc::compute( a, b, adx, ady, bdx, bdy, x, y, altDX, altDY, radius );
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
inline int64_t
matchPass( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, std::mt19937 &gen, int radius, int dir, int iter, float eps )
{
	int64_t changeCount = 0;
	if ( dir == 1 )
	{
		for ( int y = u.height() - 1; y >= 0; --y )
		{
			std::cout << "\r   iter " << iter << " minimize line " << std::setw( 4 ) << std::setfill( ' ' ) << y << std::flush;
			for ( int x = u.width() - 1; x >= 0; --x )
				changeCount += minimizePatch<BufType,DistFunc>( u, v, d, a, b, adx, ady, bdx, bdy, x, y, gen, radius, dir, eps );
		}
	}
	else
	{
		for ( int y = 0; y < u.height(); ++y )
		{
			std::cout << "\r   iter " << iter << " minimize line " << std::setw( 4 ) << std::setfill( ' ' ) << y << std::flush;
			for ( int x = 0; x < u.width(); ++x )
				changeCount += minimizePatch<BufType,DistFunc>( u, v, d, a, b, adx, ady, bdx, bdy, x, y, gen, radius, dir, eps );
		}
	}
	return changeCount;
}

template<typename BufType>
static void
runMatch( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, uint32_t srchseed, int radius, int style, int iters )
{
	std::mt19937 gen( srchseed );

	float eps = static_cast<float>( radius * 2 + 1 );
	eps *= eps;
	eps *= 0.0001F;
	switch ( static_cast<patch_style>( style ) )
	{
		case patch_style::SSD:
			matchPassInit<BufType,PatchMatchSSD>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match SSD eps " << eps << " radius " << radius << ": " << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				// if changes don't change, finished
				if ( matchPass2<BufType,PatchMatchSSD>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::SSD_GRAD:
			matchPassInit<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match SSD_GRAD eps " << eps << " radius " << radius << ": " << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
//				if ( p > 0 && p % 5 == 0 )
//				{
//					std::cout << "  running serial match" << std::endl;
//					int64_t changeCount = matchPass<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, -1, p, eps );
//					int64_t changeCount2 = matchPass<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, 1, p, eps );
//					std::cout << "\niteration " << p << " changed: " << changeCount << " and " << changeCount2 << " pixels over 2 passes" << std::endl;
//					if ( ( changeCount + changeCount2 ) == 0 )
//						break;
//				}
//				else if ( matchPass2<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				if ( matchPass2<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::SSD_GRAD_DIST:
			matchPassInit<BufType,PatchMatchSSDGradDist>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match SSD_GRAD_DIST eps " << eps << " radius " << radius << ": " << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				if ( matchPass2<BufType,PatchMatchSSDGradDist>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::GRAD:
			matchPassInit<BufType,PatchMatchGrad>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match GRAD eps " << eps << " radius " << radius << ": " << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				if ( matchPass2<BufType,PatchMatchGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
	}
}

static vector_field pmPlane( const plane &a, const plane &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	plane dist = create_plane( a.width(), a.height(), std::numeric_limits<plane::value_type>::max() );
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );
	uint32_t seedV = static_cast<uint32_t>( frameA + (frameA - frameB) - 1 );
	plane u = create_random_plane( a.width(), a.height(), seedU, 0.F, static_cast<float>( a.width() ) );
	plane v = create_random_plane( a.width(), a.height(), seedV, 0.F, static_cast<float>( a.height() ) );
//	plane u = create_iotaX_plane( a.width(), a.height() );
//	plane v = create_iotaY_plane( a.width(), a.height() );
	const_plane_buffer cA = a;
	const_plane_buffer cB = b;
	plane adx, ady, bdx, bdy;
	const_plane_buffer cAdx, cAdy, cBdx, cBdy;
	if ( static_cast<patch_style>( style ) != patch_style::SSD )
	{
		adx = central_gradient_horiz( a );
		ady = central_gradient_vert( a );
		bdx = central_gradient_horiz( b );
		bdy = central_gradient_vert( b );
		cAdx = adx;
		cAdy = ady;
		cBdx = bdx;
		cBdy = bdy;
	}
	plane_buffer fU = u;
	plane_buffer fV = v;
	plane_buffer fD = dist;
	runMatch( fU, fV, fD, cA, cB, cAdx, cAdy, cBdx, cBdy, seedU, radius, style, iters );

	return vector_field::create( std::move( u ), std::move( v ), true );
}

static vector_field pmImage( const image_buf &a, const image_buf &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	plane dist = create_plane( a.width(), a.height(), std::numeric_limits<plane::value_type>::max() );
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );
	uint32_t seedV = static_cast<uint32_t>( frameA + (frameA - frameB) - 1 );
	plane u = create_random_plane( a.width(), a.height(), seedU, 0.F, static_cast<float>( a.width() ) );
	plane v = create_random_plane( a.width(), a.height(), seedV, 0.F, static_cast<float>( a.height() ) );
	plane_buffer fU = u;
	plane_buffer fV = v;
	plane_buffer fD = dist;
	std::vector<const_plane_buffer> aB, bB;
	image_buf adx, ady, bdx, bdy;
	adx = a; ady = a;
	bdx = b; bdy = b;
	std::vector<const_plane_buffer> aBdx, aBdy, bBdx, bBdy;
	for ( size_t i = 0; i != a.size(); ++i )
	{
		if ( static_cast<patch_style>( style ) != patch_style::SSD )
		{
			adx[i] = central_gradient_horiz( a[i] );
			ady[i] = central_gradient_vert( a[i] );
			bdx[i] = central_gradient_horiz( b[i] );
			bdy[i] = central_gradient_vert( b[i] );
			aBdx.emplace_back( static_cast<const_plane_buffer>( adx[i] ) );
			aBdy.emplace_back( static_cast<const_plane_buffer>( ady[i] ) );
			bBdx.emplace_back( static_cast<const_plane_buffer>( bdx[i] ) );
			bBdy.emplace_back( static_cast<const_plane_buffer>( bdy[i] ) );
		}
		aB.emplace_back( static_cast<const_plane_buffer>( a[i] ) );
		bB.emplace_back( static_cast<const_plane_buffer>( b[i] ) );
	}
	runMatch( fU, fV, fD, aB, bB, aBdx, aBdy, bBdx, bBdy, seedU, radius, style, iters );

	return vector_field::create( std::move( u ), std::move( v ), true );
}

////////////////////////////////////////

template <typename BufType, typename DistFunc>
static bool matchRefineIter( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, std::mt19937 &gen, int radius, int iter, float eps )
{
	std::vector<size_t> counts( threading::get().size(), size_t(0) );
	size_t nChange;
	size_t totChange = 0;

	std::cout << "   iter " << iter << " left->right " << std::flush;
	threading::get().dispatch( std::bind( matchPassHorizThreadForward1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " top->bottom " << std::flush;
	threading::get().dispatch( std::bind( matchPassVertThreadDown1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.width() );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " random eps " << (eps * 100.F) << " " << std::flush;
	size_t nSeeds = static_cast<size_t>( u.height() );
	std::vector<uint32_t> seeds( nSeeds, 0 );
	for ( size_t y = 0; y < nSeeds; ++y )
		seeds[y] = gen();

	threading::get().dispatch( std::bind( matchPassRandomThread<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::cref( seeds ), std::ref( counts ), eps * 100.F ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " right->left " << std::flush;
	threading::get().dispatch( std::bind( matchPassHorizThreadBackward1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.height() );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " bottom->top " << std::flush;
	threading::get().dispatch( std::bind( matchPassVertThreadUp1<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), eps ), 0, u.width() );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " EVEN " << std::flush;
	threading::get().dispatch( std::bind( matchPassDiagonal<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), 0, eps ), 0, u.height() / 2 );
	nChange = getCount( counts );
	std::cout << nChange << std::flush;
	totChange += nChange;

	std::cout << " ODD " << std::flush;
	threading::get().dispatch( std::bind( matchPassDiagonal<BufType,DistFunc>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( u ), std::ref( v ), std::ref( d ), std::cref( a ), std::cref( b ), std::cref( adx ), std::cref( ady ), std::cref( bdx ), std::cref( bdy ), radius, std::ref( counts ), 1, eps ), 0, u.height() / 2 );
	nChange = getCount( counts );
	std::cout << nChange << std::endl;
	totChange += nChange;

	return totChange == 0;
}

template<typename BufType>
static void
matchRefine( plane_buffer &u, plane_buffer &v, plane_buffer &d, const BufType &a, const BufType &b, const BufType &adx, const BufType &ady, const BufType &bdx, const BufType &bdy, uint32_t srchseed, int radius, int style, int iters )
{
	std::mt19937 gen( srchseed );

	float eps = static_cast<float>( radius * 2 + 1 );
	eps *= eps;
	eps *= 0.0001F;
	switch ( static_cast<patch_style>( style ) )
	{
		case patch_style::SSD:
			matchPassInit<BufType,PatchMatchSSD>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match_refine SSD eps " << eps << " radius " << radius << " iters " << iters << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				// if changes don't change, finished
				if ( matchRefineIter<BufType,PatchMatchSSD>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::SSD_GRAD:
			matchPassInit<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match SSD_GRAD eps " << eps << " radius " << radius << " iters " << iters << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				if ( matchRefineIter<BufType,PatchMatchSSDGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::SSD_GRAD_DIST:
			matchPassInit<BufType,PatchMatchSSDGradDist>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match SSD_GRAD_DIST eps " << eps << " radius " << radius << " iters " << iters << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				if ( matchRefineIter<BufType,PatchMatchSSDGradDist>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
		case patch_style::GRAD:
			matchPassInit<BufType,PatchMatchGrad>( u, v, d, a, b, adx, ady, bdx, bdy, radius );
			std::cout << "\npatch_match GRAD eps " << eps << " radius " << radius << " iters " << iters << std::endl;
			for ( int p = 0; p < iters; ++p )
			{
				if ( matchRefineIter<BufType,PatchMatchGrad>( u, v, d, a, b, adx, ady, bdx, bdy, gen, radius, p, eps ) )
				{
					std::cout << "patch_match converged after " << (p + 1) << " iterations" << std::endl;
					break;
				}
			}
			break;
	}
}

static vector_field pmHierPlane( const plane &a, const plane &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );

	std::vector<plane> hierA = make_pyramid( a, "bilinear", 0.5F, 0, 100 );
	std::vector<plane> hierB = make_pyramid( b, "bilinear", 0.5F, 0, 100 );

	int levelRadius = std::max( int(2), radius - static_cast<int>( hierA.size() ) );
	vector_field prevUV;
	int refineIters = std::max( int(3), iters / 2 );
	while ( ! hierA.empty() )
	{
		plane curA = hierA.back();
		plane curB = hierB.back();
		int curW = curA.width();
		int curH = curA.height();
		hierA.pop_back();
		hierB.pop_back();
		levelRadius = std::min( radius, levelRadius + 1 );
		if ( ! prevUV.valid() )
		{
			prevUV = pmPlane( curA, curB, frameA, frameB, levelRadius, style, iters );
			continue;
		}
		
		plane adx, ady, bdx, bdy;
		const_plane_buffer cAdx, cAdy, cBdx, cBdy;
		if ( static_cast<patch_style>( style ) != patch_style::SSD )
		{
			adx = central_gradient_horiz( curA );
			ady = central_gradient_vert( curA );
			bdx = central_gradient_horiz( curB );
			bdy = central_gradient_vert( curB );
			cAdx = adx;
			cAdy = ady;
			cBdx = bdx;
			cBdy = bdy;
		}

		float scaleX = static_cast<float>( curW ) / static_cast<float>( prevUV.u().width() );
		float scaleY = static_cast<float>( curH ) / static_cast<float>( prevUV.u().height() );
		vector_field newUV = vector_field::create( resize_bicubic( prevUV.u(), curW, curH ) * scaleX,
												   resize_bicubic( prevUV.v(), curW, curH ) * scaleY,
												   true );
		plane_buffer uB = newUV.u();
		plane_buffer vB = newUV.v();
		plane d( curW, curH );
		plane_buffer dB = d;

		const_plane_buffer aB = curA;
		const_plane_buffer bB = curB;
		matchRefine( uB, vB, dB, aB, bB, cAdx, cAdy, cBdx, cBdy, seedU, levelRadius, style, refineIters );

		prevUV = newUV;
		seedU += seedU;
		refineIters = std::max( int(4), refineIters - 1 );
	}

	return prevUV;
}

static vector_field pmHierImage( const image_buf &a, const image_buf &b, int64_t frameA, int64_t frameB, int radius, int style, int iters )
{
	uint32_t seedU = static_cast<uint32_t>( frameA + (frameA - frameB) + 1 );

	std::vector<image_buf> hierA = make_pyramid( a, "bilinear", 0.5F, 0, 100 );
	std::vector<image_buf> hierB = make_pyramid( b, "bilinear", 0.5F, 0, 100 );

	int levelRadius = std::max( int(2), radius - static_cast<int>( hierA.size() ) );
	vector_field prevUV;
	int refineIters = std::max( int(4), iters * static_cast<int>( hierA.size() ) );
	while ( ! hierA.empty() )
	{
		image_buf curA = hierA.back();
		image_buf curB = hierB.back();
		int curW = curA.width();
		int curH = curA.height();
		hierA.pop_back();
		hierB.pop_back();
		levelRadius = std::min( radius, levelRadius + 1 );
		if ( ! prevUV.valid() )
		{
			prevUV = pmImage( curA, curB, frameA, frameB, levelRadius, style, refineIters * refineIters * 2 );
			refineIters -= iters;
			continue;
		}

		std::vector<const_plane_buffer> aB, bB;
		image_buf adx, ady, bdx, bdy;
		adx = curA; ady = curA;
		bdx = curB; bdy = curB;
		std::vector<const_plane_buffer> aBdx, aBdy, bBdx, bBdy;
		for ( size_t i = 0; i != a.size(); ++i )
		{
			if ( static_cast<patch_style>( style ) != patch_style::SSD )
			{
				adx[i] = central_gradient_horiz( curA[i] );
				ady[i] = central_gradient_vert( curA[i] );
				bdx[i] = central_gradient_horiz( curB[i] );
				bdy[i] = central_gradient_vert( curB[i] );
				aBdx.emplace_back( static_cast<const_plane_buffer>( adx[i] ) );
				aBdy.emplace_back( static_cast<const_plane_buffer>( ady[i] ) );
				bBdx.emplace_back( static_cast<const_plane_buffer>( bdx[i] ) );
				bBdy.emplace_back( static_cast<const_plane_buffer>( bdy[i] ) );
			}
			aB.emplace_back( static_cast<const_plane_buffer>( curA[i] ) );
			bB.emplace_back( static_cast<const_plane_buffer>( curB[i] ) );
		}

		float scaleX = static_cast<float>( curW ) / static_cast<float>( prevUV.u().width() );
		float scaleY = static_cast<float>( curH ) / static_cast<float>( prevUV.u().height() );
		vector_field newUV = vector_field::create( resize_bicubic( prevUV.u(), curW, curH ) * scaleX,
												   resize_bicubic( prevUV.v(), curW, curH ) * scaleY,
												   true );
		plane_buffer uB = newUV.u();
		plane_buffer vB = newUV.v();
		plane d( curW, curH );
		plane_buffer dB = d;

		matchRefine( uB, vB, dB, aB, bB, aBdx, aBdy, bBdx, bBdy, seedU, levelRadius, style, refineIters );

		prevUV = newUV;
		seedU += seedU;
		refineIters = std::max( int(4), refineIters - iters );
	}

	return prevUV;
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

	return vector_field( true, "p.patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////

vector_field patch_match( const image_buf &a, const image_buf &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters )
{
	engine::dimensions d;
	precondition( !a.empty() && a.dims() == b.dims(), "patch_match must have a & b of same size, received {0}x{1}x{2} and {3}x{4}x{5}", a.width(), a.height(), a.planes(), b.width(), b.height(), b.planes() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;
	return vector_field( true, "i.patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////
vector_field
hier_patch_match( const plane &a, const plane &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters )
{
	engine::dimensions d;
	precondition( a.dims() == b.dims(), "patch_match must have a & b of same size, received {0}x{1} and {3}x{4}", a.width(), a.height(), b.width(), b.height() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;

	return vector_field( true, "p.hier_patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////

vector_field
hier_patch_match( const image_buf &a, const image_buf &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters )
{
	engine::dimensions d;
	precondition( !a.empty() && a.dims() == b.dims(), "patch_match must have a & b of same size, received {0}x{1}x{2} and {3}x{4}x{5}", a.width(), a.height(), a.planes(), b.width(), b.height(), b.planes() );
	d.x = a.width();
	d.y = a.height();
	d.z = 2;
	return vector_field( true, "i.hier_patch_match", d, a, b, framenumA, framenumB, radius, static_cast<int>(style), iters );
}

////////////////////////////////////////

void
add_patchmatch( engine::registry &r )
{
	using namespace engine;
	r.add( op( "p.patch_match", pmPlane, op::threaded ) );
	r.add( op( "i.patch_match", pmImage, op::threaded ) );
	r.add( op( "p.hier_patch_match", pmHierPlane, op::threaded ) );
	r.add( op( "i.hier_patch_match", pmHierImage, op::threaded ) );
	
}

////////////////////////////////////////

} // namespace image

