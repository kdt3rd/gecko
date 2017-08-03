//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "rectangle.h"

namespace draw
{

std::weak_ptr<gl::mesh> rectangle::_mesh_cache;
gl::program::uniform rectangle::_matrix_loc;
gl::program::uniform rectangle::_color_loc;

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

void rectangle::draw( gl::api &ogl )
{
	initialize( ogl );

	auto bound = _mesh->bind();
	bound.set_uniform( _matrix_loc, _rect * ogl.current_matrix() );
	bound.set_uniform( _color_loc, _color );
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

void rectangle::initialize( gl::api &ogl )
{
	if ( !_mesh )
	{
		_mesh = _mesh_cache.lock();
		if ( !_mesh )
		{
			_mesh = std::make_shared<gl::mesh>();

			// Setup program with vertex and fragment shaders
			_mesh->set_program( new_program( ogl, "simple.vert", "single_color.frag" ) );

			// Setup vertices
			gl::vertex_buffer_data<gl::vec2> vertices;
			vertices.push_back( { 0.F, 0.F } );
			vertices.push_back( { 1.F, 0.F } );
			vertices.push_back( { 1.F, 1.F } );
			vertices.push_back( { 0.F, 1.F } );
			vertices.vbo( gl::buffer_usage::STATIC_DRAW );

			gl::element_buffer_data elements { 0, 3, 1, 1, 3, 2 };
			{
				auto bound = _mesh->bind();
				bound.vertex_attribute( "position", vertices );
				bound.set_elements( elements );
			}

			_mesh->add_triangles( 6 );
			_matrix_loc = _mesh->get_uniform_location( "matrix" );
			_color_loc = _mesh->get_uniform_location( "color" );

			_mesh_cache = _mesh;
		}
	}
}

////////////////////////////////////////

}

