
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
#include <draw/path.h>
#include <draw/rectangle.h>
#include <layout/grid.h>
#include <layout/hbox.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	std::vector<gl::color> colors = { gl::red, gl::green, gl::blue, gl::white };
	std::vector<layout::area> widgets;
	widgets.emplace_back( "w1" );
	widgets.emplace_back( "w2" );
	widgets.emplace_back( "w3" );
	widgets.emplace_back( "w4" );
	layout::area &w1 = widgets[0];
	layout::area &w2 = widgets[1];
	layout::area &w3 = widgets[2];
	layout::area &w4 = widgets[3];

	// Setup constraints for the widgets
	layout::grid lay( 2, 3 );
	lay.set_padding( 20, 10 );
	lay.set_spacing( 6, 6 );
	lay.add( w1, 0, 0 );
	lay.add( w2, 1, 0 );
	lay.add( w3, 0, 1, 2, 1 );
	lay.add( w4, 0, 2, 2, 1 );

	lay.suggest( w1.minimum_width(), 20 );
	lay.suggest( w2.minimum_width(), 20 );
	lay.suggest( w3.minimum_width(), 20 );
	lay.suggest( w4.minimum_width(), 20 );
	lay.suggest( w1.minimum_height(), 10 );
	lay.suggest( w2.minimum_height(), 10 );
	lay.suggest( w3.minimum_height(), 10 );
	lay.suggest( w4.minimum_height(), 10 );
	lay.add_constraint( w1.height() * 2 == w1.width() );
	lay.add_constraint( w3.height() * 2 == w1.height() );
	lay.add_constraint( w4.height() * 2 == w3.height() );

	// Create a window
	auto sys = platform::platform::common().create();
	auto win = sys->new_window();
	win->resize( 400, 400 );
	win->set_title( "Layout" );
	win->acquire();

	// Add variables to be adjusted manually
	lay.add_variable( lay.left() );
	lay.add_variable( lay.right() );
	lay.add_variable( lay.top() );
	lay.add_variable( lay.bottom() );
	lay.suggest( lay.left(), 0 );
	lay.suggest( lay.top(), 0 );
	lay.suggest( lay.right(), win->width() );
	lay.suggest( lay.bottom(), win->height() );

	// OpenGL information & initialization
	gl::matrix4 matrix;
	gl::api ogl;
	//ogl.setup_debugging();

	// Create rectangles for each widget
	std::vector<draw::rectangle> rects;
	for ( size_t i = 0; i < widgets.size(); ++i )
	{
		auto &w = widgets[i];
		rects.emplace_back( w.left().value(), w.top().value(), w.right().value() - w.left().value(), w.bottom().value() - w.top().value(), colors[i] );
	}

	// Render function
	win->exposed = [&]( void )
	{
		win->acquire();

		lay.suggest( lay.right(), win->width() );
		lay.suggest( lay.bottom(), win->height() );
		lay.update();

		matrix = gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) );

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		for ( size_t i = 0; i < widgets.size(); ++i )
		{
			auto &w = widgets[i];
			auto &r = rects[i];
//			std::cout << i << ": " << w.left().value() << ',' << w.top().value() << ' ' << w.right().value() - w.left().value() << 'x' << w.bottom().value() - w.top().value() << std::endl;
			r.resize( w.left().value(), w.top().value(), w.right().value() - w.left().value(), w.bottom().value() - w.top().value() );
			r.draw( ogl, matrix );
		}

		win->release();

		// Cause a redraw to continue the animation
		//win->invalidate( base::rect() );
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

