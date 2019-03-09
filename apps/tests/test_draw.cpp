// Copyright (c) 2016-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <platform/simple_window.h>
#include <gl/opengl.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/math_functions.h>
#include <draw/path.h>
#include <draw/polylines.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	auto sys = platform::platform::find_running();
	auto win = std::make_shared<platform::simple_window>( sys->new_window() );
	win->resize( 400, 400 );
	win->set_title( "Draw Test" );
	auto render_guard = win->hw_context().begin_render();
	gl::api &ogl = win->hw_context().api();
	ogl.setup_debugging();

	gl::mesh star;
	{
		// Draw a star shape (with 17 points).
		draw::path path;
		size_t p = 17;
		size_t q = 5;
		path.move_to( base::polar( 200.F, 0.F ) );
		for ( size_t i = q % p; i != 0; i = ( i + q ) % p )
			path.line_to( base::polar( 200.F, 360.0_deg * i / p ) );
		path.close();

		// Setup GL vertex/element buffers.
		gl::color color;
		size_t offset = 0;
		gl::vertex_buffer_data<gl::vec2,gl::color> pos;
		auto add_point = [&]( float x, float y )
		{
			pos.push_back( { x, y }, color );
		};

		gl::element_buffer_data tris;
		auto add_tri = [&]( size_t a, size_t b, size_t c )
		{
			tris.push_back( static_cast<uint32_t>( offset + a ), static_cast<uint32_t>( offset + b ), static_cast<uint32_t>( offset + c ) );
		};

		// Fill in the buffers
		draw::polylines lines;
		path.replay( lines );

	   	color = gl::blue;
		lines.filled( add_point, add_tri );
		offset = pos.size();

		color = gl::red;
		auto outline = lines.stroked( 2 );
		outline.filled( add_point, add_tri );

		// Finally setup the star mesh
		star.get_program().set(
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

		auto bind = star.bind();
		bind.vertex_attribute( "vertex_position", pos, 0 );
		bind.vertex_attribute( "vertex_color", pos, 1 );
		bind.set_elements( tris );

		star.add_triangles( tris.size() );
	}

	// View/projection Matrix
	gl::matrix4 matrix;
	float angle = 0.F;
	gl::program::uniform matrix_loc = star.get_uniform_location( "matrix" );
	gl::matrix4 local = gl::matrix4::translation( 200, 200 );

	// Render function
	win->exposed = [&]( void )
	{
		gl::versor rotate( angle, 0.F, 0.F, 1.F );
		matrix = gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) );

		ogl.clear();
		win->hw_context().viewport( 0, 0, win->width(), win->height() );

		{
			auto bound = star.bind();
			bound.set_uniform( matrix_loc, rotate * local * matrix );
			bound.draw();
		}

		angle += 1.0_deg;
		while ( angle > 360.0_deg )
			angle -= 360.0_deg;

		// Cause a redraw to continue the animation
		win->invalidate( platform::rect() );
	};

	// Key to take a screenshot.
	win->key_pressed = [&]( platform::event_source &, platform::scancode c )
	{
		if ( c == platform::scancode::KEY_S )
		{
			auto r = win->hw_context().begin_render();
			gl::png_write( "/tmp/test.png", static_cast<size_t>( win->width() ), static_cast<size_t>( win->height() ), 3 );
		}
	};

	win->show();

	auto dispatch = sys->get_dispatcher();
	return dispatch->execute();;
}

}

////////////////////////////////////////

int main( int argc, char *argv[] )
{
	int ret = -1;
	try
	{
		ret = safemain( argc, argv );
	}
	catch ( std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
	return ret;
}

////////////////////////////////////////
