// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "op_registry.h"
#include "plane.h"

#include <array>
#include <engine/computed_value.h>
#include <vector>

////////////////////////////////////////

namespace image
{
///
/// @brief Class image_buf provides a collection of planes
///
class image_buf : public engine::computed_base
{
public:
    typedef std::vector<plane>                 plane_list;
    typedef plane_list::iterator               iterator;
    typedef plane_list::const_iterator         const_iterator;
    typedef plane_list::reverse_iterator       reverse_iterator;
    typedef plane_list::const_reverse_iterator const_reverse_iterator;

    image_buf( void ) = default;
    template <typename... Args>
    inline image_buf(
        const base::cstring &     opname,
        const engine::dimensions &d,
        Args &&... args )
        : computed_base(
              image::op_registry(), opname, d, std::forward<Args>( args )... )
    {
        engine::dimensions pd = d;
        pd.planes             = 1;
        pd.images             = 0;
        _planes.reserve( static_cast<size_t>( d.planes ) );
        for ( engine::dimensions::value_type p = 0; p != d.planes; ++p )
            _planes.push_back( plane( "i.extract", pd, *this, size_t( p ) ) );
    }

    image_buf( const image_buf & );
    image_buf( image_buf && );
    image_buf &operator=( const image_buf & );
    image_buf &operator=( image_buf && );
    ~image_buf( void );

    inline engine::dimensions dims( void ) const
    {
        engine::dimensions r = node_dims();
        if ( r == engine::nulldim && !_planes.empty() )
        {
            r        = _planes.front().dims();
            r.planes = static_cast<engine::dimensions::value_type>( size() );
            r.images = 1;
        }
        return r;
    }
    inline int x1( void ) const { return _planes[0].x1(); }
    inline int y1( void ) const { return _planes[0].y1(); }
    inline int x2( void ) const { return _planes[0].x2(); }
    inline int y2( void ) const { return _planes[0].y2(); }
    inline int width( void ) const { return _planes[0].width(); }
    inline int height( void ) const { return _planes[0].height(); }
    inline int planes( void ) const
    {
        return static_cast<int>( _planes.size() );
    }

    void add_plane( const plane &p );
    void add_plane( plane &&p );

    inline bool   empty( void ) const { return _planes.empty(); }
    inline size_t size( void ) const { return _planes.size(); }
    inline plane &operator[]( int i )
    {
        return _planes[static_cast<size_t>( i )];
    }
    inline const plane &operator[]( int i ) const
    {
        return _planes[static_cast<size_t>( i )];
    }
    inline plane &      operator[]( size_t i ) { return _planes[i]; }
    inline const plane &operator[]( size_t i ) const { return _planes[i]; }

    inline iterator       begin( void ) { return _planes.begin(); }
    inline const_iterator begin( void ) const { return _planes.begin(); }
    inline const_iterator cbegin( void ) const { return _planes.cbegin(); }
    inline iterator       end( void ) { return _planes.end(); }
    inline const_iterator end( void ) const { return _planes.end(); }
    inline const_iterator cend( void ) const { return _planes.cend(); }

    inline reverse_iterator       rbegin( void ) { return _planes.rbegin(); }
    inline const_reverse_iterator rbegin( void ) const
    {
        return _planes.rbegin();
    }
    inline const_reverse_iterator crbegin( void ) const
    {
        return _planes.crbegin();
    }
    inline reverse_iterator       rend( void ) { return _planes.rend(); }
    inline const_reverse_iterator rend( void ) const { return _planes.rend(); }
    inline const_reverse_iterator crend( void ) const
    {
        return _planes.crend();
    }

private:
    std::vector<plane> _planes;
};

engine::hash &operator<<( engine::hash &h, const image_buf &p );

} // namespace image
