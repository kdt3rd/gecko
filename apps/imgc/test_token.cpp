
#include <iomanip>
#include <fstream>
#include <map>
#include <memory>

#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <imgproc/token.h>

using namespace imgproc;

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option( 'h', "help", "",       base::cmd_line::arg<0>, "Print help message and exit", false ),
		base::cmd_line::option(  0,  "",     "<file>, ...", base::cmd_line::args,   "File to tokenize", true )
	);

	auto option_error = base::make_guard( [&]()
	{
		std::cerr << options << std::endl;
	} );

	options.parse( argc, argv );

	option_error.dismiss();

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return 0;
	}

	for ( auto &file: options["<file>, ..."].values() )
	{
		std::ifstream str( file );
		imgproc::iterator tok( str );
		while ( tok.next() )
		{
			std::cout << tok << std::endl;
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
