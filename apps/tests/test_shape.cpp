//
// Copyright (c) 2016-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/math_functions.h>
#include <draw/shape.h>
#include <draw/quadrant.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	auto sys = platform::platform::find_running();
	auto win = sys->new_window();
	win->resize( 400, 400 );
	win->set_title( "Draw Test" );
	win->acquire();

	gl::api ogl;
	ogl.setup_debugging();

	draw::shape star;
	{
		// Draw a star shape (with 17 points).
		draw::path path;
		size_t p = 17;
		size_t q = 5;
		path.move_to( gl::vec2::polar( 200.F, 0.F ) );
		for ( size_t i = q % p; i != 0; i = ( i + q ) % p )
			path.line_to( gl::vec2::polar( 200.F, 360.0_deg * i / p ) );
		path.close();

		draw::gradient g;
		g.add_stop( 0.0, gl::blue );
		g.add_stop( 0.5, gl::green );
		g.add_stop( 1.0, gl::red );

		draw::paint paint;
		paint.set_stroke( gl::white, 2.F );
		paint.set_fill_radial( { 0, 0 }, 200, g );

		// Finally setup the star mesh
		star.create( ogl, path, paint );
	}

	draw::quadrant rect;
	{
		draw::path path;
		path.rounded_rect( { -10.F, -10.F }, 20.F, 20.F, 8.F );

		draw::paint paint;
		paint.set_stroke( gl::white, 2.F );

		rect.create( ogl, path, paint );
		rect.shape_size( 10.F, 10.F );
		rect.resize( { -50.F, -25.F, 100.F, 50.F } );
	}

	// View/projection Matrix
	float angle = 0.F;
	gl::matrix4 local = gl::matrix4::translation( 200, 200 );
	ogl.clear_color( { 0.15, 0.15, 0.15 } );

	win->release();

	// Render function
	win->exposed = [&]( void )
	{
		gl::versor rotate( angle, 0.F, 0.F, 1.F );

		ogl.viewport( 0, 0, win->width(), win->height() );
		ogl.set_projection( gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) ) );
		ogl.set_model( rotate * local );

		ogl.clear();
		star.draw( ogl );
		rect.draw( ogl );
		angle += 1.0_deg;
		while ( angle > 360.0_deg )
			angle -= 360.0_deg;

		// Cause a redraw to continue the animation
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

