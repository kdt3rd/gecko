// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

//
// Based on the following by Jukka Jylänki
//
// http://clb.demon.fi/files/RectangleBinPack.pdf
//
// and simplified version of the SkylineBinPack class in the
// provided source code:
//
// http://clb.demon.fi/files/RectangleBinPack/
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace base
{
/// @brief Pack areas into a single rectangular region
class pack
{
public:
    /// @brief An area to pack
    struct area
    {
        constexpr area( void ) {}
        constexpr area( int xx, int yy, int w, int h )
            : x( xx ), y( yy ), width( w ), height( h )
        {}

        int x      = 0;
        int y      = 0;
        int width  = 0;
        int height = 0;

        constexpr bool contains( const area &o ) const
        {
            return o.x >= x && o.y >= y && ( o.x + o.width ) <= ( x + width ) &&
                   ( o.y + o.height ) <= ( y + height );
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
            return ( width == w && height == h ) ||
                   ( width == h && height == w );
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

    /// @brief Default constructor
    pack( void );

    /// @brief Pack into the given size
    pack( int w, int h, bool allowFlipped );

    /// @brief Destructor
    ~pack( void );

    /// @brief Width of the packing area
    int width( void ) const { return _width; }

    /// @brief Height of the packing area
    int height( void ) const { return _height; }

    /// @brief Reset to pack into the given area
    void reset( int w, int h, bool allowFlipped );

    /// @brief Insert an area into the packing
    area insert( int aw, int ah );

    /// @brief Calculate occupancy
    double occupancy( void ) const;

private:
    typedef std::size_t size_t;

    bool _allowFlipped    = false;
    int  _width           = 0;
    int  _height          = 0;
    int  _usedSurfaceArea = 0;

    struct node
    {
        constexpr node( void );
        constexpr node( int xx, int yy, int w ) : x( xx ), y( yy ), width( w )
        {}

        int x     = 0;
        int y     = 0;
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

} // namespace base
