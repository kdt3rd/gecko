
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <sstream>
#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options(
		base::cmd_line::option( 'h', "help",  "",              base::cmd_line::opt_none, "Print help message and exit" ),
		base::cmd_line::option( 0,   "file",  "<file>",        base::cmd_line::opt_one,  "File to show" ),
		base::cmd_line::option( 'l', "",      "<log>",         base::cmd_line::opt_one,  "Log files" ),
		base::cmd_line::option( 'f', "frame", "<start> <end>", base::cmd_line::opt<2>,  "Frame range" ),
		base::cmd_line::option( 0,   "",      "<file>",        base::cmd_line::opt_one,  "Test file" ),
		base::cmd_line::option( 0,   "",      "<file> ...",    base::cmd_line::opt_many, "List of files to show" )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << "Usage:\n" << options; } );

	options.parse( argc, argv );

	if ( options[0] )
		std::cout << "Help!\n" << options << std::endl;

	if ( options[1] )
		std::cout << "File: " << options[1].value() << std::endl;

	if ( options[2] )
		std::cout << "Log:  " << options[2].value() << std::endl;

	if ( options[3] )
		std::cout << "Frame: " << options[3].values().at(0) << '-' << options[3].values().at(1) << std::endl;

	if ( options[4] )
		std::cout << "List: " << options[4].value() << std::endl;

	if ( options[5] )
	{
		std::cout << "List:\n";
		for ( auto &v: options[5].values() )
			std::cout << "  " << v << '\n';
	}

	std::cout << std::flush;

	errhandler.dismiss();
	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv);
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
