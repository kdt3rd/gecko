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
	: _x( 0.0 ), _y( 0.0 ), _w( 100.0 ), _h( 100.0 ), _color( c )
{
}

////////////////////////////////////////

rectangle::rectangle( float x, float y, float w, float h, const gl::color &c )
	: _x( x ), _y( y ), _w( w ), _h( h ), _color( c )
{
}

////////////////////////////////////////

void rectangle::draw( gl::api &ogl, const gl::matrix4 &m )
{
	initialize( ogl );
	auto bound = _mesh.bind();
	bound.set_uniform( _matrix_loc, ogl.current_matrix() * m );
	bound.draw();
}

////////////////////////////////////////

void rectangle::resize( float x, float y, float w, float h )
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;
}

////////////////////////////////////////

void rectangle::initialize( gl::api &ogl )
{
	if ( _init && _resized )
		return;

	if ( !_init )
	{
		// Setup program with vertex and fragment shaders
		_mesh.get_program().set(
			ogl.new_vertex_shader( R"SHADER(
				#version 330

				layout(location = 0) in vec2 vertex_position;
				layout(location = 1) in vec3 vertex_color;

				uniform mat4 matrix;

				out vec3 color;

				void main()
				{
					color = vertex_color;
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
		_vertices.push_back( { _x, _y }, _color );
		_vertices.push_back( { _x + _w, _y }, _color );
		_vertices.push_back( { _x + _w, _y + _h }, _color );
		_vertices.push_back( { _x, _y + _h }, _color );

		_vertices.vbo( gl::buffer_usage::DYNAMIC_DRAW );

		gl::element_buffer_data elements { 0, 3, 1, 1, 3, 2 };
		{
			auto bound = _mesh.bind();
			bound.vertex_attribute( "vertex_position", _vertices, 0 );
			bound.vertex_attribute( "vertex_color", _vertices, 1 );
			bound.set_elements( elements );
		}

		_mesh.add_triangles( 6 );
		_matrix_loc = _mesh.get_uniform_location( "matrix" );
	}
	else
	{
		_vertices.set( 0, { _x, _y }, _color );
		_vertices.set( 1, { _x + _w, _y }, _color );
		_vertices.set( 2, { _x + _w, _y + _h }, _color );
		_vertices.set( 3, { _x, _y + _h }, _color );
		_vertices.update();
	}

	_init = true;
}

////////////////////////////////////////

}

