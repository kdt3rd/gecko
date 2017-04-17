//
// Copyright (c) 2015-2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
		base::process proc;
		proc.set_pipe( false, true, false );
		proc.execute( "/bin/echo", { "Hello World" } );
		test.message( "pid {0}", proc.id() );

		std::istream &out = proc.std_out();
		std::string msg;
		std::getline( out, msg );

		if ( msg == "Hello World" )
			test.success( "{0}", msg );
		else
			test.failure( "{0}", msg );
	};

	test["cat"] = [&]( void )
	{
		base::process proc;
		proc.set_pipe( true, true, false );
#ifdef __APPLE__
		proc.execute( "/bin/cat", {} );
#else
		proc.execute( "/usr/bin/cat", {} );
#endif
		test.message( "pid {0}", proc.id() );
		{
			std::ostream &inp = proc.std_in();
			inp << "Hello World\n";
		}
		proc.close_in();

		std::istream &out = proc.std_out();
		std::string msg;
		std::getline( out, msg );

		if ( msg == "Hello World" )
			test.success( "{0}", msg );
		else
			test.failure( "{0}", msg );
	};

	test["error"] = [&]( void )
	{
		base::process proc;
		proc.set_pipe( true, true, true );
#ifdef __APPLE__
		proc.execute( "/bin/cat", { "/tmp/doesnt_exist" } );
#else
		proc.execute( "/usr/bin/cat", { "/tmp/doesnt_exist" } );
#endif

		test.message( "pid {0}", proc.id() );

		std::istream &err = proc.std_err();
		std::string msg;
		std::getline( err, msg );

		if ( msg.find( "No such file" ) != std::string::npos )
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
	return -1;
}
