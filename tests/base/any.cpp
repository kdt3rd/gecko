// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <base/unit_test.h>
#include <base/any.h>
#include <iostream>
#ifndef _WIN32
# include <signal.h>
#endif

namespace
{

////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "any" );

	base::cmd_line options( argv[0] );
	test.setup( options );

#ifndef _WIN32
	signal( SIGPIPE, SIG_IGN );
#endif
	try
	{
		options.parse( argc, argv );
	}
	catch ( ... )
	{
		throw_add( "parsing command line arguments" );
	}

	test["small_value"] = [&]( void )
	{
		int foo = 3;
		base::any x{ foo };
		try
		{
			base::any_cast<int &>( x ) = 42;
			test.success( "success" );
		}
		catch ( std::exception &e )
		{
			test.failure( "bad cast to int &: {0}", e.what() );
		}
		catch ( ... )
		{
			test.failure( "unknown bad cast to int &" );
		}

		try
		{
			if ( base::any_cast<const int>( x ) == 42 )
				test.success( "success" );
			else
				test.failure( "did not assign to small value member" );
		}
		catch ( std::exception &e )
		{
			test.failure( "bad cast to const int: {0}", e.what() );
		}
		catch ( ... )
		{
			test.failure( "unknown bad cast to const int" );
		}
	};

	test["change_type"] = [&]( void )
	{
		int foo = 3;
		base::any x{ foo };

		std::string bar;
		x = bar;
		if ( x.type() == typeid(std::string) )
			test.success( "success" );
		else
			test.failure( "did not change to string, type is {0}", x.type().name() );
	};

	test["large"] = [&]( void )
	{
		struct Foo
		{
			double a = 0., b = 0., c = 0.;
			uint64_t d = 0, e = 0, f = 0;
		};
		Foo foo;
		base::any x{ foo };

		try
		{
			base::any_cast<Foo &>( x ).e = 42;
			test.success( "success" );
		}
		catch ( std::exception &e )
		{
			test.failure( "bad cast to Foo &: {0}", e.what() );
		}
		catch ( ... )
		{
			test.failure( "unknown bad cast to Foo &" );
		}
		

		try
		{
			if ( base::any_cast<const Foo &>( x ).e == 42 )
				test.success( "success" );
			else
				test.failure( "did not assign to large structure member" );
		}
		catch ( std::exception &e )
		{
			test.failure( "bad cast to const Foo &: {0}", e.what() );
		}
		catch ( ... )
		{
			test.failure( "unknown bad cast to const Foo &" );
		}
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
