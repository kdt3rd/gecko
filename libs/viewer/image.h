// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/endian.h>
#include <color/color.h>
#include <draw/drawable.h>

namespace media
{
class frame;
}

namespace draw
{
enum class zoom_filter
{
    nearest,
    linear
};

////////////////////////////////////////

class image : public drawable
{
public:
    //image( void );
    //image( const std::shared_ptr<gl::texture> &t );

    void set_color_state( const ::color::state &s );
    //	void set_interleaved( gl::api &ogl,
    //						  int datax, int datay, int w, int h,
    //						  gl::image_type it, void *ptr, int nchannels,
    //						  int linestridebytes = 0,
    //						  base::endianness endian = base::endianness::NATIVE );
    //	void set_planar( gl::api &ogl,
    //					 int datax, int datay, int w, int h,
    //					 gl::image_type rit, void *red,
    //					 gl::image_type git, void *green,
    //					 gl::image_type bit, void *blue,
    //					 gl::image_type ait = gl::image_type::FLOAT, void *alpha = nullptr );

    void convert( gl::api &ogl, const media::frame &fr );
    void clear( void );

    void set_filtering( zoom_filter f );

    void set_pan( float x, float y );
    void add_zoom( float pivx, float pivy, float zoom );

    int  num_textures( void ) const;
    void set_texture_offset( int offset );

    void reset_position( float parw, float parh );

    void rebuild( platform::context &ctxt ) override;
    void draw( platform::context &ctxt ) override;

private:
    void clear_textures( void );

    void initialize( platform::context &ctxt );

    int64_t                      _dx = 0, _dy = 0;
    float                        _w = 512, _h = 512;
    std::shared_ptr<gl::texture> _texture[4];
    gl::matrix4                  _rect;
    gl::mesh                     _mesh;
    int                          _texture_offset = 0;
    bool                         _interleaved    = false;
    zoom_filter                  _filter         = zoom_filter::nearest;

    struct cache_entry
    {
        std::shared_ptr<gl::program> _interleave_prog;
        std::shared_ptr<gl::program> _planar_prog;
        gl::program::uniform         _i_tex_unit_loc;
        gl::program::uniform         _i_matrix_loc;
        gl::program::uniform         _p_num_chans;
        gl::program::uniform         _p_tex_unit0_loc;
        gl::program::uniform         _p_tex_unit1_loc;
        gl::program::uniform         _p_tex_unit2_loc;
        gl::program::uniform         _p_tex_unit3_loc;
        gl::program::uniform         _p_matrix_loc;
    };

    std::shared_ptr<cache_entry> _stash;
};

////////////////////////////////////////

} // namespace draw
