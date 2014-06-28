
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <base/contract.h>

namespace
{

int safemain( int argc, char **argv )
{
	auto sys = platform::platform::common().create();

	auto win = sys->new_window();
	win->set_title( "Hello World" );
	win->exposed.callback( [&]( void )
	{
		win->acquire();
		glViewport( 0, 0, win->width(), win->height() );
		glClearColor( 0.0, 1.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT );
		win->release();
	} );

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

