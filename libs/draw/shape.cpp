//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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

void shape::create( gl::api &ogl, const path &p, const paint &c )
{
	if ( p.empty() || c.empty() )
		return;

	polylines lines;
	p.replay( lines );

	// Setup the stroke rendering.
	if ( c.get_stroke_width() != 0.0 )
	{
		_stroke_matrix_loc = stroke_mesh( ogl, _stroke, c );
		lines.stroked( c.get_stroke_width() ).filled( _stroke, "position" );
	}
	else
		_stroke.clear();

	_fill_matrix_loc = fill_mesh( ogl, _fill, c );
	_fill_texture = get_fill_texture( ogl, c );

	if ( c.has_fill() )
		lines.filled( _fill, "position" );
	else
		_fill.clear();
}

////////////////////////////////////////

void shape::draw( gl::api &ogl )
{
	// Draw fill
	if ( _fill.valid() )
	{
		gl::texture::binding t;
		if ( _fill_texture )
			t = _fill_texture->bind();
		auto b = _fill.bind();
		b.set_uniform( _fill_matrix_loc, ogl.current_matrix() );
		b.draw();
	}

	// Draw stroke
	if ( _stroke.valid() )
	{
		auto b = _stroke.bind();
		b.set_uniform( _stroke_matrix_loc, ogl.current_matrix() );
		b.draw();
	}
}

////////////////////////////////////////

}

