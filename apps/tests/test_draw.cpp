
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <base/contract.h>
#include <base/math_functions.h>
#include <draw/canvas.h>
#include <draw/object.h>
#include <draw/color_wheel.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	auto sys = platform::platform::common().create();

	auto screens = sys->screens();

	for ( auto scr: screens )
		std::cout << "Screen " << scr->bounds() << ' ' << scr->dpi() << std::endl;

	auto win = sys->new_window();
	win->set_title( "Draw Test" );
	win->exposed.function() = [&]( void )
	{
		win->acquire();
		glViewport( 0, 0, static_cast<GLsizei>(win->width()), static_cast<GLsizei>(win->height()) );
		{
			auto canvas = std::make_shared<draw::canvas>();
			canvas->clear_color( base::color( 0.75F, 0.75F, 0.75F ) );
			canvas->clear();
			canvas->ortho( 0, float(win->width()), 0, float(win->height()) );

			base::paint paint( { 1.0, 1.0, 1.0 }, 2 );
			paint.set_fill_color( { 0, 0, 0 } );

			// Draw star
			{
				using namespace base::math;
				base::point p;
				base::path path( { 50, 10 } );
				double side = 100;
				path.line_by( base::point::polar( side, 108_deg ) );
				path.line_by( base::point::polar( side, -36_deg ) );
				path.line_by( base::point::polar( side, 180_deg ) );
				path.line_by( base::point::polar( side, 36_deg ) );
				path.close();
				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}

			// Draw a square with a hole
			{
				base::point p;
				base::path path;
				path.rectangle( { { 200, 10 }, { 100, 100 } } );
				path.rectangle( { { 220, 30 }, { 60, 60 } }, true );
				path.rectangle( { { 240, 50 }, { 20, 20 } } );
				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}

			// Draw colorwheel
			{
				draw::color_wheel wheel;
				wheel.create( canvas, base::point( win->width()/2.0, win->height()/2.0 ), 50 );
				wheel.draw( *canvas );
			}
		}
		win->release();
	};
	win->resized.function() = [&]( double w, double h )
	{
		std::cout << "Resized to: " << w << 'x' << h << std::endl;
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

