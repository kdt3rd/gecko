
#include <base/uri.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <web/server.h>

std::string hello( const std::string &n );

namespace
{

int safemain( int argc, char *argv[] )
{
	web::server test( 8080 );
	test.resource( "GET", "/hello" ) = []( web::response &resp, web::request &req )
	{
		resp << hello( "World" );
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
