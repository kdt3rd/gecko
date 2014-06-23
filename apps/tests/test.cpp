
#include <iostream>
#include <unistd.h>

#include <base/contract.h>
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>

namespace {

int safemain( int argc, char **argv )
{
	auto sys = platform::platform::common().create();

	auto win = sys->new_window();
	win->exposed.callback( [&]( void )
	{
		auto c = win->canvas();
		glViewport( 0, 0, win->width(), win->height() );
		c->clear_color( { 0, 1, 0 } );
		c->clear();
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

