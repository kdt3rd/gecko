///
/// @author Kimball Thurston
///
/// Based on the following by Jukka Jylänki
///
/// http://clb.demon.fi/files/RectangleBinPack.pdf
///
/// and simplified version of the SkylineBinPack class in the
/// provided source code:
///
/// http://clb.demon.fi/files/RectangleBinPack/
///


#pragma once

#include <vector>
#include <cstdint>
#include <cstdlib>

namespace base
{

///
/// @brief Class pack provides...
///
class pack
{
public:
	struct area
	{
		constexpr area( void ) {}
		constexpr area( int xx, int yy, int w, int h ) : x( xx ), y( yy ), width( w ), height( h ) {}

		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		constexpr bool contains( const area &o ) const
		{
			return o.x >= x && o.y >= y && (o.x + o.width) <= (x + width) && (o.y + o.height) <= (y + height);
		}

		constexpr bool fits_normal( int w, int h ) const
		{
			return w <= width && h <= height;
		}

		constexpr bool fits_flipped( int w, int h ) const
		{
			return w != h && w <= height && h <= width;
		}

		constexpr bool matches( int w, int h ) const
		{
			return ( width == w && height == h ) || ( width == h && height == w );
		}

		constexpr bool flipped( int w, int h ) const
		{
			return w != h && width == h && height == w;
		}

		inline int score( int w, int h ) const
		{
			return std::min( std::abs( width - w ), std::abs( height - h ) );
		}

		inline void clear( void ) { x = y = width = height = 0; }
		inline bool empty( void ) const { return width == 0 && height == 0; }
	};
	
	pack( void );
	pack( int w, int h, bool allowFlipped );
	~pack( void );

	int width( void ) const { return _width; }
	int height( void ) const { return _width; }

	void reset( int w, int h, bool allowFlipped );

	area insert( int aw, int ah );

	double occupancy( void ) const;

private:
	typedef std::size_t size_t;

	bool _allowFlipped = false;
	int _width = 0;
	int _height = 0;
	uint64_t _usedSurfaceArea = 0;

	struct node
	{
		constexpr node( void );
		constexpr node( int xx, int yy, int w ) : x( xx ), y( yy ), width( w ) {}

		int x = 0;
		int y = 0;
		int width = 0;
	};

	std::vector<node> _skyLine;
	std::vector<area> _usedRects;
	std::vector<area> _freeRects;

	bool fits( size_t i, int w, int h, int &y, int &wastedArea ) const;
	area find_min_waste( int aw, int ah, int &bestH, int &bestWA, size_t &idx );

	void add( size_t idx, const area &a );
	void merge( void );

	// same as guillotine function insert w/
	// true, rectbestshortsidefit, splitmaximizearea
	area waste_insert( int aw, int ah );
	void add_waste( size_t idx, int w, int h, int y );

};

}

