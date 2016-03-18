
#include <platform/platform.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	// Create a window
	auto sys = platform::platform::common().create();
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

	// Create a triangle mesh
	gl::mesh triangle;
	{
		// Vertex and fragment shaders
		triangle.set_program(
			ogl.new_shader( gl::shader::type::VERTEX, R"SHADER(
				#version 330

				layout(location = 0) in vec3 vertex_position;
				layout(location = 1) in vec3 vertex_colour;

				uniform mat4 matrix;

				out vec3 colour;

				void main()
				{
					colour = vertex_colour;
					gl_Position = matrix * vec4( vertex_position, 1.0 );
				}
			)SHADER" ),
			ogl.new_shader( gl::shader::type::FRAGMENT, R"SHADER(
				#version 330

				in vec3 colour;
				out vec4 frag_colour;

				void main()
				{
					frag_colour = vec4( colour, 1.0 );
				}
			)SHADER" )
		);

		// Vertex data with 2 attributes (position and color).
		gl::vertex_buffer_data<gl::vec3,gl::color> data
		{
			{ { 0.0F, 0.5F, 0.0F }, { 1.0F, 0.0F, 0.0F } },
			{ { 0.5F,-0.5F, 0.0F }, { 0.0F, 1.0F, 0.0F } },
			{ {-0.5F,-0.5F, 0.0F }, { 0.0F, 0.0F, 1.0F } }
		};
		triangle.vertex_attribute( 0, data, 0 );
		triangle.vertex_attribute( 1, data, 1 );
	}

	// Matrix for animating the triangle
	gl::matrix4 matrix;
	gl::program::uniform matrix_loc = triangle.get_uniform_location( "matrix" );
	float speed = 0.01F;

	// Called to draw the window
	win->exposed = [&]( void )
	{
		// Update the animation
		if ( std::abs( matrix.get( 0, 3 ) ) > 1.F )
			speed = -speed;
		matrix.translate_x( speed );

		win->acquire();

		// Clear the window
		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		// Draw the triangle
		triangle.use();
		triangle.set_uniform( matrix_loc, matrix );
		triangle.draw( gl::primitive::TRIANGLES, 0, 3 );

		win->release();
		win->invalidate( base::rect() );
	};

	// Key to take a screenshot.
	win->key_pressed = [&]( const std::shared_ptr<platform::keyboard> &, platform::scancode c )
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

