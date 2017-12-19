//
// Copyright (c) 2016-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <platform/platform.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	// Create a window
	//const char *disp = argv[1];
	auto sys = platform::platform::find_running();
	auto win = sys->new_window();
	win->set_title( "Triangle" );
	win->acquire();

	// OpenGL information & initialization
	gl::api ogl;
	std::cout << "OpenGL version " << ogl.get_version() << std::endl;
	std::cout << "Vendor: " << ogl.get_vendor() << std::endl;
	std::cout << "Renderer: " << ogl.get_renderer() << std::endl;
	ogl.setup_debugging();
	ogl.enable( gl::capability::DEPTH_TEST );
	ogl.depth_func( gl::depth_test::LESS );
	ogl.enable( gl::capability::CULL_FACE );
	//ogl.clear_color( gl::color( 0.2, 0.4, 0.1 ) );

	// Create a triangle mesh
	gl::mesh triangle;
	{
		// Setup program with vertex and fragment shaders
		triangle.get_program().set(
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
			auto tbind = triangle.bind();
			tbind.vertex_attribute( "vertex_position", data, 0 );
			tbind.vertex_attribute( "vertex_color", data, 1 );
			tbind.set_elements( elements );
		}

		/// Add triangle (3 points).
		triangle.add_triangles( 3 );
	}

	// Matrix for animating the triangle.
	gl::matrix4 matrix;
	gl::program::uniform matrix_loc = triangle.get_uniform_location( "matrix" );
	float speed = 0.01F;

	win->release();

	auto animtimer = sys->create_timer();
	animtimer->elapsed = [&]() { win->invalidate( platform::rect() ); };
	animtimer->reset_repeat( 0.1 );
	sys->get_dispatcher()->add_waitable( animtimer );
	animtimer->activate( true );

	// Called to draw the window
	win->exposed = [&]( void )
	{
		// Update the animation
		if ( std::abs( matrix.get( 0, 3 ) ) > 1.F )
			speed = -speed;
		matrix.translate_x( speed );

		// Clear the window
		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		// Draw the triangle
		{
			auto bound = triangle.bind();
			bound.set_uniform( matrix_loc, matrix );
			bound.draw();
		}

		// Cause a redraw to continue the animation
//		win->invalidate( base::rect() );
	};

	// Key to take a screenshot.
	win->key_pressed = [&]( platform::event_source &, platform::scancode c )
	{
		if ( c == platform::scancode::KEY_S )
		{
			win->acquire();
			gl::png_write( "/tmp/test.png", static_cast<size_t>( win->width() ), static_cast<size_t>( win->height() ), 3 );
			win->release();
		}
	};

	// Finally, display the window.
	win->show();

	// Run the event dispatcher until exit.
	auto dispatch = sys->get_dispatcher();
	return dispatch->execute();
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

