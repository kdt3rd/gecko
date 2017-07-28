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

void rectangle::resize( float x, float y, float w, float h )
{
	_rect = gl::matrix4::scaling( w, h );
	_rect.translate_x( x );
	_rect.translate_y( y );
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
			_mesh->get_program().set(
				ogl.new_vertex_shader( R"SHADER(
					#version 330

					layout(location = 0) in vec2 vertex_position;

					uniform mat4 matrix;
					uniform vec4 mesh_color;

					out vec3 color;

					void main()
					{
						color = vec3( mesh_color );
						gl_Position = matrix * vec4( vertex_position, 0.0, 1.0 );
					}
				)SHADER" ),
				ogl.new_fragment_shader( R"SHADER(
					#version 330

					in vec3 color;
					out vec4 frag_color;

					void main()
					{
						frag_color = vec4( color, 1.0 );
					}
				)SHADER" )
			);

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
				bound.vertex_attribute( "vertex_position", vertices );
				bound.set_elements( elements );
			}

			_mesh->add_triangles( 6 );
			_matrix_loc = _mesh->get_uniform_location( "matrix" );
			_color_loc = _mesh->get_uniform_location( "mesh_color" );

			_mesh_cache = _mesh;
		}
	}
}

////////////////////////////////////////

}

