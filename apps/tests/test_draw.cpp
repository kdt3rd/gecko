
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/math_functions.h>
#include <draw/path.h>
#include <draw/polylines.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	auto sys = platform::platform::common().create();
	auto win = sys->new_window();
	win->resize( 400, 400 );
	win->set_title( "Draw Test" );
	win->acquire();

	gl::api ogl;
	ogl.setup_debugging();

	gl::mesh star;
	{
		// Draw a star shape (with 17 points).
		gl::vec2 center { 200, 200 };
		double side = 180;
		size_t p = 17;
		size_t q = 5;

		draw::path path;
		path.move_to( center + gl::vec2::polar( side, 0.F ) );
		for ( size_t i = q % p; i != 0; i = ( i + q ) % p )
			path.line_to( center + gl::vec2::polar( side, base::degree( 360.F ) * i / p ) );
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

	// Render function
	win->exposed = [&]( void )
	{
		win->acquire();

		gl::versor rotate( angle, 0.F, 0.F, 1.F );
		matrix = gl::matrix4::ortho( 0, win->width(), 0, win->height() );
		matrix = matrix * gl::matrix4::rotation( rotate );

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		{
			auto bound = star.bind();
			bound.set_uniform( matrix_loc, matrix );
			bound.draw();
		}

		angle += base::degree( 1.F );
		while ( angle > base::degree( 360.F ) )
			angle -= base::degree( 360.F );

		win->release();

		// Cause a redraw to continue the animation
		win->invalidate( base::rect() );
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

