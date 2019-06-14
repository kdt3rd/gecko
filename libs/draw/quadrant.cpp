// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "quadrant.h"

#include "polylines.h"
#include "shaders.h"

namespace draw
{
////////////////////////////////////////

quadrant::quadrant( void ) {}

////////////////////////////////////////

void quadrant::add( gl::api &ogl, const polylines &lines, const paint &c )
{
    if ( lines.empty() || c.empty() )
        return;

    if ( c.has_fill() )
    {
        mesh m;
        m.matrix  = fill_mesh( ogl, m.msh, c, "quadrant.vert" );
        m.shape   = m.msh.get_uniform_location( "shape" );
        m.resize  = m.msh.get_uniform_location( "resize" );
        m.topleft = m.msh.get_uniform_location( "top_left" );
        m.tex     = get_fill_texture( ogl, c );

        lines.filled( m.msh, "position" );
        _meshes.push_back( std::move( m ) );
    }

    if ( c.get_stroke_width() > dim( 0 ) )
    {
        mesh m;
        m.matrix  = stroke_mesh( ogl, m.msh, c, "quadrant.vert" );
        m.shape   = m.msh.get_uniform_location( "shape" );
        m.resize  = m.msh.get_uniform_location( "resize" );
        m.topleft = m.msh.get_uniform_location( "top_left" );

        lines.stroked( c.get_stroke_width() ).filled( m.msh, "position" );
        _meshes.push_back( std::move( m ) );
    }
}

////////////////////////////////////////

void quadrant::rebuild( platform::context & ) { _meshes.clear(); }

////////////////////////////////////////

void quadrant::draw( platform::context &ctxt )
{
    gl::api &ogl = ctxt.api();
    for ( auto &m: _meshes )
    {
        if ( m.msh.valid() )
        {
            gl::texture::binding t;
            if ( m.tex )
                t = m.tex->bind();
            auto b = m.msh.bind();
            b.set_uniform( m.matrix, ogl.current_matrix() );
            b.set_uniform( m.shape, _shape );
            b.set_uniform( m.resize, _resize );
            b.set_uniform( m.topleft, _top_left );
            b.draw();
        }
    }
}

////////////////////////////////////////

} // namespace draw
