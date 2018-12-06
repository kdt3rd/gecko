//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include <base/bounded_safe_queue.h>
#include <base/contract.h>
#include <base/unit_test.h>
#include <base/cmd_line.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#ifndef _WIN32
#include <signal.h>
#endif

namespace
{


void producer( base::bounded_safe_queue<size_t> &q, size_t n, size_t id )
{
	unsigned int seed = id;
	unsigned int xx = rand_r( &seed ) % 100;
	while ( xx > 0 )
	{
		std::this_thread::yield();
		--xx;
	}

	for ( size_t i = 0; i != n; ++i )
	{
		while ( ! q.try_push( i ) )
			std::this_thread::yield();
	}
}

void consumer( base::bounded_safe_queue<size_t> &q, size_t n, size_t id )
{
	unsigned int seed = id;
	unsigned int xx = rand_r( &seed ) % 100;
	while ( xx > 0 )
	{
		std::this_thread::yield();
		--xx;
	}

	size_t sum = 0;
	for ( size_t i = 0; i != n; ++i )
	{
		size_t outv;
		while ( ! q.try_pop( outv ) )
			std::this_thread::yield();
		sum += outv;
	}
	std::cout << "consumer " << (id - 234) << ": sum " << sum << std::endl;
}

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "bounded_safe_queue" );

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

	test["mpmc_small"] = [&]( void )
	{
		// hrm, how do we actually test that we get what we want?
		std::vector<std::thread> producers;
		std::vector<std::thread> consumers;
		base::bounded_safe_queue<size_t> q( 4 );
		producers.resize( 4 );
		for ( size_t i = 0; i != 4; ++i )
			producers[i] = std::thread( &producer, std::ref( q ), size_t(1000000), i + 100 );
		consumers.resize( 2 );
		for ( size_t i = 0; i != 2; ++i )
			consumers[i] = std::thread( &consumer, std::ref( q ), size_t(2000000), i + 234 );
		for ( size_t i = 0; i != 4; ++i )
			producers[i].join();
		for ( size_t i = 0; i != 2; ++i )
			consumers[i].join();
		test.success( "4 producers, 2 consumers, 4000000 elements" );
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
