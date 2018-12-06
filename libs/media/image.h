//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <base/small_vector.h>
#include <base/endian.h>
#include <color/state.h>

#include "metadata.h"

namespace base
{
class allocator;
}

////////////////////////////////////////

namespace media
{

class image_buffer;

/// TODO: add packing (i.e. dpx 10-bit, 12-bit)
struct plane_layout
{
    base::endianness _endian = base::endianness::NATIVE;
	int8_t _bits = -1;
	int8_t _ysubsample = 1;
	int8_t _ysubsample_shift = 0;
	int8_t _xsubsample = 1;
	int8_t _xsubsample_shift = 0;
	bool _floating = false;
	bool _unsigned = true;
};

///
/// @brief Class image provides...
///
class image
{
    /// not copyable, meant to be subclassed and carried in a smart pointer
    image( const image & ) = delete;
    image &operator=( const image & ) = delete;
    image( image && ) = delete;
    image &operator=( image & ) = delete;

public:
    using plane_list = base::small_vector<std::string, 4>;
    image( void );
    image( const area_rect &area );
    virtual ~image( void );

    bool interleaved( void ) const { return storage_interleaved(); }

    /// TODO: add level
    //bool has_mipmap( void ) const ;
    //bool is_tiled( void ) const;
    /// TODO:
    //int tile_x( void ) const;
    //int tile_y( void ) const;
    //int tile_z( void ) const;

    /// this is informational only, but can be used to control the
    /// processing, such that the reader can help determine whether to
    /// process in tiles, single scanline or scanline chunks
    /// (i.e. openexr has tiled, single scanline, 16-scanline chunks)
    std::pair<int, int> preferred_chunk_size() const { return compute_preferred_chunk(); }

    /// The "full" resolution of the image
    inline const area_rect &full_area( void ) const { return _full_area; }
    /// Active (data) window - where there are real pixels
    inline const area_rect &active_area( void ) const { return _active_area; }

    /// aspect ratio for pixels (square or not)
    inline float aspect_ratio( void ) const { return _pix_aspect_ratio; }
    inline void aspect_ratio( float ar ) { _pix_aspect_ratio = ar; }

    inline const color::state &color_state( void ) const { return _color_state; }
    void color_state( const color::state &s );

    inline bool empty( void ) const { return _planes.empty(); }

    inline size_t size( void ) const { return _planes.size(); }
    plane_list available_planes( void ) const;
    const std::string &plane_name( size_t p ) const { return at( p )._name; }
    plane_layout layout( size_t p ) const { return at( p )._layout; }
    double outside_value( size_t p ) const { return at( p )._outside; }

    /// utility function to create image buffers for the entire active
    /// area. This may create an overlapped set of image_buffers that
    /// use the same large memory block if the underlying format is
    /// interleaved. Or if the image is planar, it will create
    /// separate buffers.
    ///
    /// This routine is probably not the most efficient way to access
    /// image data for processing, where it is more memory efficient
    /// to access by scanline or tile. Instead, a consumer of this
    /// image probably wants to allocate memory itself and pass that
    /// in. This places a burden on the consumer to conform to the
    /// plane layout, but is provided as a default path to get to as
    /// close to zero-copy imaging as possible.
    ///
    /// @param planes place to store the image_buffers. if the incoming
    ///               list is not empty, will re-use the image_buffer if
    ///               they are the correct layout
    /// @param a allocator to use when creating memory for the image_buffer
    /// @param preferred_chunk only creates the image buffer large
    ///                        enough for the preferred processing size.
    void create_buffers( std::vector<image_buffer> &planes,
                         base::allocator &a,
                         bool preferred_chunk ) const;

    void extract_plane( size_t plane, image_buffer &pbuf );
    void extract_image( std::vector<image_buffer> &planes );

	inline void set_meta( base::cstring name, metadata_value v ) { _metadata[name] = std::move( v ); }
	inline const metadata &meta( void ) const { return _metadata; }

protected:
    virtual bool storage_interleaved( void ) const = 0;
    virtual void fill_plane( size_t plane, image_buffer &buffer ) = 0;
    virtual void fill_image( std::vector<image_buffer> &planes ) = 0;
    /// by default, returns the entire area
    virtual std::pair<int64_t, int64_t> compute_preferred_chunk( void ) const;

    /// sets both full and active area to the same value
    void set_area( const area_rect &r ) { _full_area = r; _active_area = r; }
    /// sets only the full area
    void set_full_area( const area_rect &r ) { _full_area = r; }
    /// sets only the active area (data window)
    void set_active_area( const area_rect &r ) { _active_area = r; }

    void register_plane( std::string name, const plane_layout &pl, double outsideval )
    {
        _planes.emplace_back( std::move( name ), pl, outsideval );
    }

private:
    struct plane_info
    {
        plane_info( std::string n, const plane_layout &pl, double ov )
            : _name( std::move( n ) ), _layout( pl ), _outside( ov )
        {}
        std::string _name;
        plane_layout _layout;
        double _outside;
    };
    inline const plane_info &at( size_t p ) const { return _planes.at( p ); }

    using plane_store = base::small_vector<plane_info, 4>;

    area_rect _full_area;
    area_rect _active_area;

    float _pix_aspect_ratio = 1.f;
    color::state _color_state;

    plane_store _planes;

    metadata _metadata;
};

} // namespace media



