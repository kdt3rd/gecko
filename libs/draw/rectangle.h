// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "drawable.h"

#include <gl/color.h>
#include <gl/matrix4.h>
#include <gl/mesh.h>

namespace draw
{
////////////////////////////////////////

class rectangle : public drawable
{
public:
    rectangle( const color &c = gl::white );
    rectangle( dim x, dim y, dim w, dim h, const color &c = gl::white );

    void rebuild( platform::context &ctxt ) override;
    void draw( platform::context &ctxt ) override;

    void set_size( dim w, dim h );
    void set_position( dim x, dim y );

    void set_color( const color &c ) { _color = c; }

private:
    void initialize( platform::context &ctxt );

    gl::matrix4 _rect;
    color       _color;

    struct cache_entry
    {
        gl::mesh             _mesh;
        gl::program::uniform _matrix_loc;
        gl::program::uniform _color_loc;
    };

    std::shared_ptr<cache_entry> _stash;
};

////////////////////////////////////////

} // namespace draw
