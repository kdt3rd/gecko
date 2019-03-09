// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <base/small_vector.h>
#include <base/endian.h>
#include <color/state.h>

#include "metadata.h"

////////////////////////////////////////

namespace media
{

///
/// @brief Class data provides...
///
class data
{
public:
    using plane_list = base::small_vector<std::string, 4>;

    data( void );
    data( const area_rect &area );
	virtual ~data( void );

    /// this is informational only, but can be used to control the
    /// processing, such that the reader can help determine whether to
    /// process in tiles, single scanline or scanline chunks
    /// (i.e. openexr has tiled, single scanline, 16-scanline chunks)
    std::pair<int, int> preferred_chunk_size() const { return compute_preferred_chunk(); }

    /// The "full" resolution of the image
    const area_rect &full_area( void ) const { return _full_area; }
    /// Active (data) window - where there are real pixels
    const area_rect &active_area( void ) const { return _active_area; }

    inline float aspect_ratio( void ) const { return _pix_aspect_ratio; }
    inline void aspect_ratio( float ar ) { _pix_aspect_ratio = ar; }

    inline const color::state &color_state( void ) const { return _color_state; }
    void color_state( const color::state &s );

    size_t size( void ) const { return _planes.size(); }
    plane_list available_planes( void ) const;
    const std::string &plane_name( size_t p ) const { return at( p ); }

    // TODO: add other types beside float, customize deep?
    void retrieve_line( size_t plane, int64_t y, std::vector<float> &buffer )
    { fill_plane_line( plane, y, buffer ); }

	inline void set_meta( base::cstring name, metadata_value v ) { _metadata[name] = std::move( v ); }
	inline const metadata &meta( void ) const { return _metadata; }

protected:
    virtual bool storage_interleaved( void ) const = 0;
    virtual void fill_plane_line( size_t plane, int64_t y, std::vector<float> &buffer ) = 0;
    /// by default, returns the entire area
    virtual std::pair<int64_t, int64_t> compute_preferred_chunk( void ) const;

    /// sets both full and active area to the same value
    void set_area( const area_rect &r ) { _full_area = r; _active_area = r; }
    /// sets only the full area
    void set_full_area( const area_rect &r ) { _full_area = r; }
    /// sets only the active area (data window)
    void set_active_area( const area_rect &r ) { _active_area = r; }

    void register_plane( std::string name )//, const plane_layout &pl )
    {
        _planes.emplace_back( std::move( name ) );
    }

private:
    using plane_store = base::small_vector<std::string, 4>;
    inline const std::string &at( size_t p ) const { return _planes.at( p ); }

    area_rect _full_area;
    area_rect _active_area;

    float _pix_aspect_ratio = 1.f;

    color::state _color_state;

    plane_store _planes;
    metadata _metadata;
};

} // namespace media



