//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/file_system.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/uri.h>
#include <base/unit_test.h>

#include <fstream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "stream" );

	base::cmd_line options( argv[0] );
	test.setup( options );

	// Create a test file
	{
		std::ofstream out( "/tmp/test.txt" );
		out << "Hello World" << std::endl;
		out.close();
	}

	try
	{
		options.parse( argc, argv );
	}
	catch ( ... )
	{
		throw_add( "parsing command line arguments" );
	}

	test["system_stream"] = [&]( void )
	{
		std::ifstream in( "/tmp/test.txt" );
		char buf[8];
		while ( !in.eof() )
		{
			in.read( buf, 8 );
		}
	};

	test["posix_stream"] = [&]( void )
	{
		auto fs = base::file_system::get( std::string( "file" ) );
		base::istream in = fs->open_read( base::uri( base::cstring( "/tmp/test.txt" ) ) );
		char buf[8];
		while ( !in.eof() )
		{
			in.read( buf, 8 );
		}
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
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
	return -1;
}
