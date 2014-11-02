
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <sstream>
#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option( 'h', "help",  "",              base::cmd_line::arg<0>,   "Print help message and exit", false ),
		base::cmd_line::option(  0,  "file",  "<file>",        base::cmd_line::arg<1>,   "File to show", false ),
		base::cmd_line::option( 'l', "",      "[<log>]",       base::cmd_line::arg<0,1>, "Log files", false ),
		base::cmd_line::option( 'v', "",      "",              base::cmd_line::counted,  "Verbose (can be specified many times)", false ),
		base::cmd_line::option( 'r', "range", "<start> <end>", base::cmd_line::arg<2>,   "Frame range", false ),
		base::cmd_line::option(  0,  "",      "<arg>",         base::cmd_line::arg<1>,   "Test file", true ),
		base::cmd_line::option(  0,  "",      "<file> ...",    base::cmd_line::args,     "List of files to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );

	options.parse( argc, argv );

	if ( options["help"] )
	{
		std::cout << "Help!\n" << options << std::endl;
		errhandler.dismiss();
	}

	if ( auto &opt = options["file"] )
		std::cout << "File: " << opt.value() << std::endl;

	if ( auto &opt = options["l"] )
		std::cout << "Log:  " << ( opt.count() == 1 ? options[2].value() : "" ) << std::endl;

	if ( auto &opt = options["v"] )
		std::cout << "Verbose:" << opt.count() << std::endl;

	if ( auto &opt = options["range"] )
		std::cout << "Range:" << opt.values().at(0) << '-' << options[4].values().at(1) << std::endl;

	if ( auto &opt = options["<arg>"] )
		std::cout << "Arg:  " << opt.value() << std::endl;

	if ( auto &opt = options["<file> ..."] )
	{
		std::cout << "List:\n";
		for ( auto &v: opt.values() )
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
