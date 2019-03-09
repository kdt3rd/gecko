// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/small_vector.h>
#include <base/contract.h>
#include <base/unit_test.h>
#include <base/cmd_line.h>
#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "small_vector" );
	base::cmd_line options( argv[0] );
	test.setup( options );

	try
	{
		options.parse( argc, argv );
	}
	catch ( ... )
	{
		throw_add( "parsing command line arguments" );
	}

	test["basic_small_vector"] = [&]( void )
	{
		base::small_vector<int, 8> vi;
		vi.push_back( 1 );
		if ( vi.size() == 1 )
			test.success( "push_back" );
		else
			test.failure( "push_back size != 1" );
		if ( vi.is_small() )
			test.success( "is_small" );
		else
			test.failure( "is_small returns false" );
		if ( vi[0] == 1 )
			test.success( "accessor[0]" );
		else
			test.failure( "accessor[0] != 1" );

		vi.insert( vi.end(), { 2, 3, 4, 5, 6, 7, 8 } );
		if ( vi.size() == 8 )
			test.success( "append" );
		else
			test.failure( "append size != 8" );
		if ( vi.is_small() )
			test.success( "is_small" );
		else
			test.failure( "is_small returns false" );

		vi.push_back( 9 );
		if ( vi.size() == 9 )
			test.success( "push_back2" );
		else
			test.failure( "push_back2 size != 9" );
		if ( vi.is_small() )
			test.failure( "switch_to_allocated failed" );
		else
			test.success( "switch_to_allocated" );
		int matchcount = 0;
		for ( size_t i = 0; i != vi.size(); ++i )
		{
			if ( vi[i] == int(i+1) )
				++matchcount;
		}
		if ( matchcount == 9 )
			test.success( "switch_to_allocated move" );
		else
			test.failure( "switch_to_allocated move values don't match" );

		vi.clear();
		if ( vi.empty() )
			test.success( "clear" );
		else
			test.failure( "clear failed" );
		vi.shrink_to_fit();
		if ( vi.is_small() )
			test.success( "shrink_to_fit" );
		else
			test.failure( "shrink_to_fit failed" );
		std::cerr << "TODO: add tons more tests..." << std::endl;
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
}

} // empty namespace


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
