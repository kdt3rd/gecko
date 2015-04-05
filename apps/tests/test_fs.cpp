
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/posix_file_system.h>
#include <sstream>
#include <iostream>

#include <unistd.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<dir> ...",    base::cmd_line::args,     "List of directories to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	base::file_system::add( "file", std::make_shared<base::posix_file_system>() );

	if ( auto &opt = options["<dir> ..."] )
	{
		for ( auto &v: opt.values() )
		{
			base::uri path( v );
			std::cout << "Path = " << v << '\n';
			auto dir = base::file_system::get( path )->readdir( path );
			while ( ++dir )
				std::cout << *dir << std::endl;
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
