
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
	win->set_title( "Draw Test" );
	win->acquire();

	gl::api ogl;
	ogl.setup_debugging();
	ogl.enable( gl::capability::DEPTH_TEST );
	ogl.depth_func( gl::depth_test::LESS );
//	ogl.enable( gl::capability::CULL_FACE );

	gl::mesh star;
	{
		using namespace base::math;
		gl::vec2 center { 500, 500 };
		double side = 450;
		std::vector<gl::vec2> points;
		size_t p = 12;
		size_t q = 5;
		for ( size_t i = 0; i < p; ++i )
			points.push_back( center + gl::vec2::polar( side, 360_deg * i / p ) );

		draw::path path;
		size_t i = q % points.size();
		path.move_to( points[0] );
		while( i != 0 )
		{
			path.line_to( points[i] );
			i = ( i + q ) % points.size();
		}
		path.close();

		draw::polylines lines;
		path.replay( lines );

		gl::vertex_buffer_data<gl::vec2> pos;
		auto add_point = [&]( float x, float y )
		{
			pos.push_back( { x, y } );
		};

		gl::element_buffer_data tris;
		auto add_tri = [&]( size_t a, size_t b, size_t c )
		{
			tris.push_back( static_cast<uint32_t>( a ), static_cast<uint32_t>( b ), static_cast<uint32_t>( c ) );
		};

		lines.filled( add_point, add_tri );

		star.get_program().set(
			ogl.new_vertex_shader( R"SHADER(
				#version 330

				layout(location = 0) in vec3 position;

				uniform mat4 matrix;

				void main()
				{
					gl_Position = matrix * vec4( position, 1.0 );
				}
			)SHADER" ),
			ogl.new_fragment_shader( R"SHADER(
				#version 330

				out vec4 frag_color;

				void main()
				{
					frag_color = vec4( 0.0, 0.0, 1.0, 1.0 );
				}
			)SHADER" )
		);

		{
			auto bind = star.bind();
			bind.vertex_attribute( "position", pos, 0 );
			bind.set_elements( tris );
		}
	}

	gl::matrix4 matrix;
	gl::program::uniform matrix_loc = star.get_uniform_location( "matrix" );

	win->exposed = [&]( void )
	{
		win->acquire();

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		{
			auto bound = star.bind();
			bound.set_uniform( matrix_loc, matrix );
			bound.draw();
		}

		win->release();
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

