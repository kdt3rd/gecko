
#include <platform/platform.h>
#include <gl/api.h>
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

	// OpenGL initialization
	gl::api ogl;
	std::cout << "OpenGL version " << ogl.get_version() << std::endl;
	std::cout << "Vendor: " << ogl.get_vendor() << std::endl;
	std::cout << "Renderer: " << ogl.get_renderer() << std::endl;

	ogl.setup_debugging();

	ogl.enable( gl::capability::DEPTH_TEST );
	ogl.depth_func( gl::depth_test::LESS );

	// Create the geometry for the triangle
	auto vbo_points = ogl.new_array_buffer<float>( {
		0.0F,  0.5F,   0.0F,
		0.5F, -0.5F,   0.0F,
		-0.5F, -0.5F,   0.0F
	} );

	auto vbo_colors = ogl.new_array_buffer<float>( {
		1.0F, 0.0F,  0.0F,
		0.0F, 1.0F,  0.0F,
		0.0F, 0.0F,  1.0F
	} );

	auto vao = ogl.new_vertex_array();
	{
		auto tmp = vao->bind();
		tmp.attrib_pointer( 0, vbo_points, 3 );
		tmp.attrib_pointer( 1, vbo_colors, 3 );
	}

	// The shaders and program for the triangle
	auto vshader = ogl.new_shader( gl::shader::type::VERTEX, R"SHADER(
		#version 410

		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec3 vertex_colour;

		out vec3 colour;

		void main()
		{
			colour = vertex_colour;
			gl_Position = vec4( vertex_position, 1.0 );
		}
	)SHADER" );

	auto fshader = ogl.new_shader( gl::shader::type::FRAGMENT, R"SHADER(
		#version 410

		in vec3 colour;
		out vec4 frag_colour;

		void main()
		{
			frag_colour = vec4( colour, 1.0 );
		}
	)SHADER" );

	auto prog = ogl.new_program( vshader, fshader );

	// Called to draw the window
	win->exposed = [&]( void )
	{
		win->acquire();

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		prog->use();
		auto triangle = vao->bind();
		triangle.draw( gl::primitive::TRIANGLES, 0, 3 );

		win->release();
	};

	win->key_pressed = [&]( const std::shared_ptr<platform::keyboard> &, platform::scancode c )
	{
		if ( c == platform::scancode::KEY_S )
		{
			win->acquire();
			gl::png_write( "/tmp/test.png", static_cast<size_t>( win->width() ), static_cast<size_t>( win->height() ), 3 );
			win->release();
		}
	};

	// Display the window
	win->show();

	// Run the event dispatcher
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

