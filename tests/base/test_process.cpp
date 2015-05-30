
#include <base/contract.h>
#include <base/unit_test.h>
#include <base/cmd_line.h>
#include <base/process.h>
#include <iostream>
#include <signal.h>

namespace
{

////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "process" );

	base::cmd_line options( argv[0] );
	test.setup( options );

	signal( SIGPIPE, SIG_IGN );

	try
	{
		options.parse( argc, argv );
	}
	catch ( ... )
	{
		throw_add( "parsing command line arguments" );
	}

	test["echo"] = [&]( void )
	{
		base::process proc( "/bin/echo", { "-n", "Hello World" } );
		test.message( "pid {0}", proc.id() );
		std::istream &out = proc.std_out();
		char c = out.get();
		std::string msg;
		while ( out )
		{
			msg.push_back( c );
			c = out.get();
		}
		if ( msg == "Hello World" )
			test.success( "{0}", msg );
		else
			test.failure( "{0}", msg );
	};

	test["cat"] = [&]( void )
	{
		base::process proc( "/usr/bin/cat", {} );
		test.message( "pid {0}", proc.id() );
		{
			std::ostream &inp = proc.std_in();
			inp << "Hello World";
		}
		proc.close_in();

		std::istream &out = proc.std_out();

		char c = out.get();
		std::string msg;
		while ( out )
		{
			msg.push_back( c );
			c = out.get();
		}
		if ( msg == "Hello World" )
			test.success( "{0}", msg );
		else
			test.failure( "{0}", msg );
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
}

////////////////////////////////////////

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
