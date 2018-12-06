//
// Copyright (c) 2016-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include <platform/platform.h>
#include <platform/simple_window.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <draw/rectangle.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	// Create a window
	auto sys = platform::platform::find_running();
	auto win = std::make_shared<platform::simple_window>( sys->new_window() );

	win->set_title( "Triangle" );
	auto render_guard = win->hw_context().begin_render();
	gl::api &ogl = win->hw_context().api();
	ogl.setup_debugging();

	draw::rectangle rect( 50, 50, 100, 100, gl::blue );

	// Matrix for setting up ortho view
	float w = 100;
	float dw = 4;

	// Called to draw the window
	win->exposed = [&]( void )
	{

		// Clear the window
		ogl.clear();
		win->hw_context().viewport( 0, 0, win->width(), win->height() );
		ogl.set_projection( gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) ) );

		// Draw the rectangle
		rect.set_position( 50, 50 );
		rect.set_size( w, 100 );
		rect.draw( win->hw_context() );

		// Cause a redraw to continue the animation
		win->invalidate( platform::rect() );

		w += dw;
		if ( w > 200 )
			dw = -4;
		else if ( w < 100 )
			dw = 4;
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
