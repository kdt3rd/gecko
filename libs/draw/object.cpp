//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "object.h"
#include "polylines.h"
#include "shaders.h"

namespace draw
{

////////////////////////////////////////

object::object( void )
{
}

////////////////////////////////////////

void object::create( gl::api &ogl, const path &p, const paint &c )
{
	if ( p.empty() || c.empty() )
		return;

	polylines lines;
	p.replay( lines );

	// Setup the stroke rendering.
	if ( c.get_stroke_width() != 0.0 )
	{
		_stroke.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( "color_mesh.vert" ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "single_color.frag" ) ) );
		_stroke.get_program().use();
		_stroke.set_uniform( "color", c.get_stroke_color() );
		_stroke_matrix_loc = _stroke.get_uniform_location( "matrix" );

		gl::vertex_buffer_data<gl::vec2> points;
		gl::element_buffer_data tris;
		{
			auto add_point = [&]( float cx, float cy )
			{
				points.push_back( { cx, cy } );
			};

			auto add_tri = [&]( size_t a, size_t b, size_t c )
			{
				tris.push_back( a, b, c );
			};

			lines.stroked( c.get_stroke_width() ).filled( add_point, add_tri );

			auto bind = _stroke.bind();
			bind.vertex_attribute( "position", points );
			bind.set_elements( tris );
			_stroke.add_triangles( tris.size() );
		}
	}
	else
		_stroke.clear();

	if ( c.has_fill_color() )
	{
		_fill.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( "position_uv.vert" ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "single_color.frag" ) ) );
		_fill.get_program().use();
		_fill.set_uniform( "color", c.get_fill_color() );
		_fill_texture.reset();
	}
	else if ( c.has_fill_linear() )
	{
		_fill.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( "position_uv.vert" ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "linear_gradient.frag" ) ) );
		_fill.get_program().use();
		_fill.set_uniform( "txt", 0 );
		_fill.set_uniform( "origin", c.get_fill_linear_origin() );
		_fill.set_uniform( "dir", c.get_fill_linear_size() );
		_fill_texture = new_gradient( ogl, c.get_fill_linear_gradient() );
		_fill_texture->bind().set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	}
	else if ( c.has_fill_radial() )
	{
		_fill.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( "position_uv.vert" ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "radial_gradient.frag" ) ) );
		_fill.get_program().use();
		_fill.set_uniform( "txt", 0 );
		_fill.set_uniform( "center", c.get_fill_radial_p1() );
		_fill.set_uniform( "radius", c.get_fill_radial_r2() );
		_fill_texture = new_gradient( ogl, c.get_fill_radial_gradient() );
		_fill_texture->bind().set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	}
	else if ( c.has_fill_conical() )
	{
		_fill.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( "position_uv.vert" ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "conical_gradient.frag" ) ) );
		_fill.get_program().use();
		_fill.set_uniform( "txt", 0 );
		_fill.set_uniform( "center", c.get_fill_conical_center() );
		_fill_texture = new_gradient( ogl, c.get_fill_conical_gradient() );
		_fill_texture->bind().set_wrapping( gl::wrapping::REPEAT );
	}
	else if ( c.has_no_fill() )
	{
		_fill.clear();
		_fill_texture.reset();
	}
	else
		throw std::runtime_error( "unhandled fill type" );

	if ( !c.has_no_fill() )
	{
		gl::vertex_buffer_data<gl::vec2> points;
		gl::element_buffer_data tris;
		{
			auto add_point = [&]( float cx, float cy )
			{
				points.push_back( { cx, cy } );
			};

			auto add_tri = [&]( size_t a, size_t b, size_t c )
			{
				tris.push_back( a, b, c );
			};

			lines.filled( add_point, add_tri );

			auto bind = _fill.bind();
			bind.vertex_attribute( "position", points );
			bind.set_elements( tris );
			_fill.add_triangles( tris.size() );
			_fill_matrix_loc = _fill.get_uniform_location( "matrix" );
		}
	}
	else
		_fill.clear();
}

////////////////////////////////////////

void object::draw( gl::api &ogl )
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

