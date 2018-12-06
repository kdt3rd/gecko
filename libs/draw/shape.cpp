//
// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "shape.h"
#include "polylines.h"
#include "shaders.h"

namespace draw
{

////////////////////////////////////////

shape::shape( void )
{
}

////////////////////////////////////////

void shape::add( gl::api &ogl, const polylines &lines, const paint &c )
{
	if ( lines.empty() || c.empty() )
		return;

	if ( c.has_fill() )
	{
		mesh m;
		m.matrix = fill_mesh( ogl, m.msh, c, "position_uv.vert" );
		m.tex = get_fill_texture( ogl, c );

		lines.filled( m.msh, "position" );
		_meshes.push_back( std::move( m ) );
	}

	if ( c.get_stroke_width() > dim(0) )
	{
		mesh m;
		m.matrix = stroke_mesh( ogl, m.msh, c, "position_uv.vert" );

		lines.stroked( c.get_stroke_width() ).filled( m.msh, "position" );
		_meshes.push_back( std::move( m ) );
	}
}

////////////////////////////////////////

void shape::rebuild( platform::context &ctxt )
{
	_meshes.clear();
}

////////////////////////////////////////

void shape::draw( platform::context &ctxt )
{
	gl::api &ogl = ctxt.api();

	ogl.save_matrix();
	ogl.model_matrix().scale( ( _resize[0] / _shape[0] ).count(),
	                          ( _resize[1] / _shape[1] ).count() );
	ogl.model_matrix().translate( to_api( _top_left[0] ), to_api( _top_left[1] ) );

	for ( auto &m: _meshes )
	{
		if ( m.msh.valid() )
		{
			gl::texture::binding t;
			if ( m.tex )
				t = m.tex->bind();
			auto b = m.msh.bind();
			b.set_uniform( m.matrix, ogl.current_matrix() );
			b.draw();
		}
	}

	ogl.restore_matrix();
}

////////////////////////////////////////

}
