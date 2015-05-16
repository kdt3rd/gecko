
#include <base/uri.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <web/client.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<uri> ...",    base::cmd_line::args,     "List of URI to connect to", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	if ( auto &opt = options["<uri> ..."] )
	{
		web::client test( "tester" );
		for ( auto &u: opt.values() )
		{
			base::uri path( u );
			test.get( path );
		}
	}

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
