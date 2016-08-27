
#include <platform/platform.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <draw/rectangle.h>

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
	ogl.setup_debugging();

	draw::rectangle rect( 50, 50, 100, 100, gl::blue );

	// Matrix for setting up ortho view
	gl::matrix4 matrix;
	float w = 100;
	float dw = 4;

	// Called to draw the window
	win->exposed = [&]( void )
	{
		win->acquire();

		matrix = gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) );

		// Clear the window
		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		// Draw the rectangle
		rect.resize( 50, 50, w, 100 );
		rect.draw( ogl, matrix );

		win->release();

		// Cause a redraw to continue the animation
		win->invalidate( base::rect() );

		w += dw;
		if ( w > 200 )
			dw = -4;
		else if ( w < 100 )
			dw = 4;
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
