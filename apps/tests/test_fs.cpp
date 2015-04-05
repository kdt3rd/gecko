
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/posix_file_system.h>
#include <sstream>
#include <iostream>

namespace
{

int safemain( void )
{
	base::file_system::add( "file", std::make_shared<base::posix_file_system>() );
	base::uri path( "file", "", "tmp" );

	auto dir = base::file_system::get( path )->readdir( path );
	while ( ++dir )
		std::cout << *dir << std::endl;

	return 0;
}

}

int main( void )
{
	try
	{
		return safemain();
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
