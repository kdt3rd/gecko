//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "rectangle.h"

namespace draw
{

////////////////////////////////////////

rectangle::rectangle( const gl::color &c )
	: _color( c )
{
	_rect = gl::matrix4::scaling( 100, 100 );
}

////////////////////////////////////////

rectangle::rectangle( float x, float y, float w, float h, const gl::color &c )
	: _color( c )
{
	_rect = gl::matrix4::scaling( w, h );
	_rect.translate_x( x );
	_rect.translate_y( y );
}

////////////////////////////////////////

void rectangle::rebuild( platform::context &ctxt )
{
	_stash.reset();
}

////////////////////////////////////////

void rectangle::draw( platform::context &ctxt )
{
	initialize( ctxt );

	auto bound = _stash->_mesh.bind();
	bound.set_uniform( _stash->_matrix_loc, _rect * ctxt.api().current_matrix() );
	bound.set_uniform( _stash->_color_loc, _color );
	bound.draw();
}

////////////////////////////////////////

void rectangle::set_size( float w, float h )
{
	_rect.get( 0, 0 ) = w;
	_rect.get( 1, 1 ) = h;
}

////////////////////////////////////////

void rectangle::set_position( float x, float y )
{
	_rect.get( 0, 3 ) = x;
	_rect.get( 1, 3 ) = y;
}

////////////////////////////////////////

void rectangle::initialize( platform::context &ctxt )
{
	if ( !_stash )
	{
		if ( ctxt.retrieve_common( this, _stash ) )
		{
			// new object
			//
			// Setup program with vertex and fragment shaders
			_stash->_mesh.set_program( new_program( ctxt.api(), "simple.vert", "single_color.frag" ) );

			// Setup vertices
			gl::vertex_buffer_data<gl::vec2> vertices;
			vertices.push_back( { 0.F, 0.F } );
			vertices.push_back( { 1.F, 0.F } );
			vertices.push_back( { 1.F, 1.F } );
			vertices.push_back( { 0.F, 1.F } );
			vertices.vbo( gl::buffer_usage::STATIC_DRAW );

			gl::element_buffer_data elements { 0, 3, 1, 1, 3, 2 };
			{
				auto bound = _stash->_mesh.bind();
				bound.vertex_attribute( "position", vertices );
				bound.set_elements( elements );
			}

			_stash->_mesh.add_triangles( 6 );
			_stash->_matrix_loc = _stash->_mesh.get_uniform_location( "matrix" );
			_stash->_color_loc = _stash->_mesh.get_uniform_location( "color" );
		}
	}
}

////////////////////////////////////////

}

