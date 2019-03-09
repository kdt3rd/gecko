// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT

#include "triangle.h"

namespace draw
{

////////////////////////////////////////

triangle::triangle( void )
{
}

////////////////////////////////////////

void triangle::draw( gl::api &ogl, const gl::matrix4 &m )
{
	initialize( ogl );
	auto bound = _mesh.bind();
	bound.set_uniform( _matrix_loc, m );
	bound.draw();
}

////////////////////////////////////////

void triangle::initialize( gl::api &ogl )
{
	if ( _init )
		return;

	// Setup program with vertex and fragment shaders
	_mesh.get_program().set(
		ogl.new_vertex_shader( R"SHADER(
			#version 330

			layout(location = 0) in vec3 vertex_position;
			layout(location = 1) in vec3 vertex_color;

			uniform mat4 matrix;

			out vec3 color;

			void main()
			{
				color = vertex_color;
				gl_Position = matrix * vec4( vertex_position, 1.0 );
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

	// Vertex data with 2 attributes (position and color).
	gl::vertex_buffer_data<gl::vec3,gl::color> data
	{
		{ { 0.5F,-0.5F, 0.0F }, { 0.0F, 1.0F, 0.0F } },
		{ { 0.0F, 0.5F, 0.0F }, { 1.0F, 0.0F, 0.0F } },
		{ {-0.5F,-0.5F, 0.0F }, { 0.0F, 0.0F, 1.0F } }
	};

	// List of indices for the triangle.
	gl::element_buffer_data elements { 0, 1, 2 };

	// Bind the elements data and vertex data to the attributes.
	{
		auto tbind = _mesh.bind();
		tbind.vertex_attribute( "vertex_position", data, 0 );
		tbind.vertex_attribute( "vertex_color", data, 1 );
		tbind.set_elements( elements );
	}

	/// Add triangle (3 points).
	_mesh.add_triangles( 3 );

	_matrix_loc = _mesh.get_uniform_location( "matrix" );
	_init = true;
}

////////////////////////////////////////

}

