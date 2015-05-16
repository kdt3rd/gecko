
#include <base/uri.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <web/server.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	/*
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<uri> ...",    base::cmd_line::args,     "List of URI to connect to", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();
	*/

	web::server test( 8080 );
	test.default_resource( "GET" ) = []( web::response &resp, web::request &req )
	{
		std::cout << "Sending response!" << std::endl;
		resp << "Hello World!\n";
	};

	test.run();

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
