// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "drawable.h"
#include "paint.h"
#include "path.h"
#include "polylines.h"

#include <gl/api.h>
#include <gl/mesh.h>
#include <list>

namespace draw
{
////////////////////////////////////////

class quadrant : public drawable
{
public:
    quadrant( void );
    quadrant( float x, float y, float w, float h )
    {
        set_position( x, y );
        set_size( w, h );
    }

    void add( gl::api &ogl, const polylines &p, const paint &c );

    void add( gl::api &ogl, const polylines &p, const gl::color &c )
    {
        add( ogl, p, paint( c, 1.F ) );
    }

    void add( gl::api &ogl, const path &p, const paint &c )
    {
        if ( p.empty() || c.empty() )
            return;

        polylines lines;
        p.replay( lines );
        add( ogl, lines, c );
    }

    void add( gl::api &ogl, const path &p, const gl::color &c )
    {
        add( ogl, p, paint( c, 1.F ) );
    }

    void clear( void ) { _meshes.clear(); }

    void rebuild( platform::context &ogl ) override;
    void draw( platform::context &ogl ) override;

    void shape_size( float w, float h ) { _shape.set( w, h ); }

    void set_size( float w, float h ) { _resize.set( w, h ); }

    void set_position( float x, float y ) { _top_left.set( x, y ); }

private:
    gl::vec2 _shape;
    gl::vec2 _resize;
    gl::vec2 _top_left;

    struct mesh
    {
        gl::mesh                     msh;
        std::shared_ptr<gl::texture> tex;
        gl::program::uniform         matrix;
        gl::program::uniform         shape;
        gl::program::uniform         resize;
        gl::program::uniform         topleft;
    };

    std::list<mesh> _meshes;
};

////////////////////////////////////////

} // namespace draw
