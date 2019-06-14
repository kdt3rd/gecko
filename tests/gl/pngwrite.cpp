// Copyright (c) 2016-2017 Ian Godin
// SPDX-License-Identifier: MIT

#include <base/cmd_line.h>
#include <base/unit_test.h>
#include <gl/api.h>
#include <gl/check.h>
#include <gl/framebuffer.h>
#include <gl/png_image.h>
#include <gl/texture.h>
#include <platform/platform.h>

namespace
{
int safemain( int argc, char *argv[] )
{
    base::cmd_line options( argv[0] );

    base::unit_test test( "info" );
    test.setup( options );

    options.add_help();
    options.parse( argc, argv );

    if ( options["help"] )
    {
        std::cerr << options << std::endl;
        return -1;
    }

    // Create a window
    auto sys = platform::platform::common().create();
    auto win = sys->new_window();

    win->set_title( "Triangle" );
    win->acquire();

    // OpenGL initialization
    gl::api ogl;
    //	ogl.enable( gl::capability::DEPTH_TEST );
    //	ogl.depth_func( gl::depth_test::LESS );

    // Create the geometry for the triangle
    auto vbo_points = ogl.new_array_buffer<float>(
        { 0.0F, 0.5F, 0.0F, 0.5F, -0.5F, 0.0F, -0.5F, -0.5F, 0.0F } );

    auto vbo_colors = ogl.new_array_buffer<float>(
        { 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F } );

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

    checkgl();

    gl::texture     txt;
    gl::framebuffer fb;
    auto            bfb = fb.bind();
    {
        auto bound = txt.bind( gl::texture::target::TEXTURE_RECTANGLE );
        bound.image_2d_rgb(
            gl::format::RGB, 200, 200, gl::image_type::UNSIGNED_BYTE, nullptr );
        checkgl();

        checkgl();

        bfb.attach( txt );
        checkgl();
    }

    test["png_write"] = [&]( void ) {
        checkgl();

        ogl.clear( gl::buffer_bit::COLOR_BUFFER_BIT );

        checkgl();

        ogl.viewport(
            0, 0, static_cast<size_t>( 200 ), static_cast<size_t>( 200 ) );

        checkgl();

        prog->use();

        checkgl();

        auto triangle = vao->bind();
        triangle.draw( gl::primitive::TRIANGLES, 0, 3 );

        checkgl();

        gl::png_write( "/tmp/test.png", 200, 200, 3 );

        checkgl();

        win->release();
    };

    test.run( options );
    test.clean();

    return -static_cast<int>( test.failure_count() );
}

} // namespace

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
