//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "quadrant.h"
#include "polylines.h"
#include "shaders.h"

namespace draw
{

////////////////////////////////////////

quadrant::quadrant( void )
{
}

////////////////////////////////////////

void quadrant::create( gl::api &ogl, const polylines &lines, const paint &c )
{
	if ( lines.empty() || c.empty() )
		return;

	// Setup the stroke rendering.
	if ( c.get_stroke_width() != 0.0 )
	{
		_stroke_matrix_loc = stroke_mesh( ogl, _stroke, c, "quadrant.vert" );
		_stroke_shape_loc = _stroke.get_uniform_location( "shape" );
		_stroke_resize_loc = _stroke.get_uniform_location( "resize" );
		_stroke_topleft_loc = _stroke.get_uniform_location( "top_left" );

		lines.stroked( c.get_stroke_width() ).filled( _stroke, "position" );
	}
	else
		_stroke.clear();

	if ( c.has_fill() )
	{
		_fill_matrix_loc = fill_mesh( ogl, _fill, c, "quadrant.vert" );
		_fill_shape_loc = _fill.get_uniform_location( "shape" );
		_fill_resize_loc = _fill.get_uniform_location( "resize" );
		_fill_topleft_loc = _fill.get_uniform_location( "top_left" );
		_fill_texture = get_fill_texture( ogl, c );

		lines.filled( _fill, "position" );
	}
	else
		_fill.clear();
}

////////////////////////////////////////

void quadrant::draw( gl::api &ogl )
{
	// Draw fill
	if ( _fill.valid() )
	{
		gl::texture::binding t;
		if ( _fill_texture )
			t = _fill_texture->bind();
		auto b = _fill.bind();
		b.set_uniform( _fill_matrix_loc, ogl.current_matrix() );
		b.set_uniform( _fill_shape_loc, _shape );
		b.set_uniform( _fill_resize_loc, _resize );
		b.set_uniform( _fill_topleft_loc, _top_left );
		b.draw();
	}

	// Draw stroke
	if ( _stroke.valid() )
	{
		auto b = _stroke.bind();
		b.set_uniform( _stroke_matrix_loc, ogl.current_matrix() );
		b.set_uniform( _stroke_shape_loc, _shape );
		b.set_uniform( _stroke_resize_loc, _resize );
		b.set_uniform( _stroke_topleft_loc, _top_left );
		b.draw();
	}
}

////////////////////////////////////////

void quadrant::resize( const base::rect &r )
{
	_top_left.set( r.x(), r.y() );
	_resize.set( r.width(), r.height() );
}

////////////////////////////////////////

}

