
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/unit_test.h>
#include <sstream>
#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "file",  "<file>",        base::cmd_line::arg<1>,   "File to show", false ),
		base::cmd_line::option( 'l', "",      "[<log>]",       base::cmd_line::arg<0,1>, "Log files", false ),
		base::cmd_line::option( 'v', "",      "",              base::cmd_line::counted,  "Verbose (can be specified many times)", false ),
		base::cmd_line::option( 'r', "range", "<start> <end>", base::cmd_line::arg<2>,   "Frame range", false ),
		base::cmd_line::option(  0,  "",      "<arg>",         base::cmd_line::arg<1>,   "Test file", true ),
		base::cmd_line::option(  0,  "",      "<file> ...",    base::cmd_line::args,     "List of files to show", true )
	);

	base::unit_test test( "cmd_line" );
	test.setup( options );

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.add_help();
	options.parse( argc, argv );
	errhandler.dismiss();

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return -1;
	}

	test["single"] = [&]( void )
	{
		if ( auto &opt = options["file"] )
			test.success( "file option is {0}", opt.value() );
		else
			test.success( "file option was not specified" );
	};

	test["optional"] = [&]( void )
	{
		if ( auto &opt = options["l"] )
		{
			if ( opt.count() == 1 )
				test.success( "log option is '{0}'", opt[0] );
			else
				test.success( "log option is empty" );
		}
		else
			test.success( "log option was not specified" );
	};

	test["verbose"] = [&]( void )
	{
		if ( auto &opt = options["v"] )
			test.success( "verbose was specified {0} times", opt.count() );
		else
			test.success( "verbose was not specified" );
	};

	test["multi"] = [&]( void )
	{
		if ( auto &opt = options["range"] )
			test.success( "range specified as {0} to {1}", opt[0], opt[1] );
		else
			test.success( "range not specified" );
	};

	test["required"] = [&]( void )
	{
		if ( auto &opt = options["<arg>"] )
			test.success( "Argument is {0}", opt.value() );
		else
			test.failure( "Argument is missing" );
	};

	test["list"] = [&]( void )
	{
		if ( auto &opt = options["<file> ..."] )
			test.success( "List is {0}", base::infix_separated( ", ", opt.values() ) );
		else
			test.failure( "List is missing" );
	};

	test.run();
	test.clean();

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
	return -1;
}
