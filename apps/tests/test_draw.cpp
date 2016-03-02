
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/math_functions.h>
#include <draw/canvas.h>
#include <draw/object.h>
#include <draw/color_wheel.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
#if 1
	auto sys = platform::platform::common().create();

	auto screens = sys->screens();

	for ( auto scr: screens )
		std::cout << "Screen " << scr->bounds() << ' ' << scr->dpi() << std::endl;

	auto win = sys->new_window();
	win->set_title( "Draw Test" );
	win->exposed = [&]( void )
	{
		win->acquire();
		glViewport( 0, 0, static_cast<GLsizei>(win->width()), static_cast<GLsizei>(win->height()) );
		{
			auto canvas = std::make_shared<draw::canvas>();
			canvas->clear_color( base::color( 0.75F, 0.75F, 0.75F ) );
			canvas->clear();
			canvas->ortho( 0, float(win->width()), 0, float(win->height()) );

			base::paint paint( { 1.0, 1.0, 1.0 }, 10 );
//			paint.set_fill_color( { 0, 0, 0 } );

#if 0
			// Draw a simple square
			{
				base::point p;
				base::path path;
				path.rectangle( { { 10, 150 }, { 100, 100 } } );
				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}
#endif

#if 1
			// Draw star
			{
				using namespace base::math;
				base::point center { 500, 500 };
				double side = 450;
				std::vector<base::point> points;
				size_t p = 12;
				size_t q = 5;
				for ( size_t i = 0; i < p; ++i )
					points.push_back( center + base::point::polar( side, 360_deg * double(i) / double(p) ) );

				base::path path;
				size_t i = q % points.size();
				path.move_to( points[0] );
				while( i != 0 )
				{
					path.line_to( points[i] );
					i = ( i + q ) % points.size();
				}

				path.close();

				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}
#endif

#if 0

			// Draw a square with a hole
			{
				base::point p;
				base::path path;
				path.rectangle( { { 250, 10 }, { 100, 100 } } );
				path.rectangle( { { 270, 30 }, { 60, 60 } } );
				path.rectangle( { { 290, 50 }, { 20, 20 } } );
				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}

			// Draw a square with a hole
			{
				base::point p;
				base::path path;
				path.rectangle( { { 130, 10 }, { 100, 100 } } );
				path.rectangle( { { 150, 30 }, { 60, 60 } }, true );
				path.rectangle( { { 170, 50 }, { 20, 20 } } );
				draw::object obj;
				obj.create( canvas, path, paint );
				obj.draw( *canvas );
			}
#endif

			// Draw colorwheel
			{
				/*
				draw::color_wheel wheel;
				wheel.create( canvas, base::point( win->width()/2.0, win->height()/2.0 ), 50 );
				wheel.draw( *canvas );
				*/
			}
		}
		win->release();
	};

	win->show();

	auto dispatch = sys->get_dispatcher();
	return dispatch->execute();;
#endif
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

