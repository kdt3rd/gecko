//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/contract.h>
#include <base/unit_test.h>
#include <base/semaphore.h>
#include <base/shared_mutex.h>
#include <base/event.h>
#include <iostream>
#ifndef _WIN32
# include <signal.h>
#endif
#include <thread>
#include <atomic>

namespace
{

template <typename T>
struct sem_test_struct
{
	static std::atomic<int> cnt;

	using sem_type = T;
	static void thread_counter( sem_type &s )
	{
		for ( int i = 0; i < 10000; ++i )
		{
			std::this_thread::yield();
			s.signal();
			std::this_thread::yield();
		}
	}

	static void thread_bump1( sem_type &s )
	{
		while ( true )
		{
			s.wait();
			if ( cnt == 10000 )
				break;
			++cnt;
		}
	}
};

template <typename T>
std::atomic<int> sem_test_struct<T>::cnt;

static int shareCount = 0;
void shared_writer( base::shared_mutex &m )
{
	while ( true )
	{
		std::this_thread::yield();
		{
			std::unique_lock<base::shared_mutex> lk( m );
			++shareCount;
			if ( shareCount == 10000 )
				break;
		}
		std::this_thread::yield();
	}
}

void shared_reader( base::shared_mutex &m )
{
	while ( true )
	{
		std::this_thread::yield();
		{
			base::shared_lock_guard<base::shared_mutex> lk( m );
			if ( shareCount == 10000 )
				break;
		}
		std::this_thread::yield();
		{
			base::shared_lock<base::shared_mutex> lk( m );
			if ( shareCount == 10000 )
				break;
		}
		std::this_thread::yield();
	}
}

void ev_1( base::event &e1, base::event &e2 )
{
//	std::cout << "ev_1 waiting e1" << std::endl;
	e1.wait();
//	std::cout << "ev_1 lower e1" << std::endl;
	e1.lower();
//	std::cout << "ev_1 raise e2" << std::endl;
	e2.raise();
//	std::cout << "ev_1 waiting e1" << std::endl;
	e1.wait();
//	std::cout << "ev_1 done" << std::endl;
}

void ev_2( base::event &e1, base::event &e2 )
{
//	std::cout << "ev_2 raising e1" << std::endl;
	e1.raise();
//	std::cout << "ev_2 waiting e2" << std::endl;
	e2.wait();
//	std::cout << "ev_2 lower e2" << std::endl;
	e2.lower();
//	std::cout << "ev_2 raise e1" << std::endl;
	e1.raise();
//	std::cout << "ev_2 done" << std::endl;
}

void auto_e1( base::auto_reset_event &e1, base::auto_reset_event &e2 )
{
//	std::cout << "auto_e1 wait" << std::endl;
	e1.wait();
//	std::cout << "auto_e1 set e2" << std::endl;
	e2.set();
//	std::cout << "auto_e1 wait e1" << std::endl;
	e1.wait();
//	std::cout << "auto_e1 done" << std::endl;
}

void auto_e2( base::auto_reset_event &e1, base::auto_reset_event &e2 )
{
//	std::cout << "auto_e2 set e1" << std::endl;
	e1.set();
//	std::cout << "auto_e2 wait" << std::endl;
	e2.wait();
//	std::cout << "auto_e2 set e1" << std::endl;
	e1.set();
//	std::cout << "auto_e2 done" << std::endl;
}

////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "locking" );

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

	test["simple_semaphore"] = [&]( void )
	{
		base::simple_semaphore s;
		using stest = sem_test_struct<base::simple_semaphore>;
		stest::cnt = int(0);
		std::thread t1( stest::thread_bump1, std::ref( s ) );
		std::thread t2( stest::thread_bump1, std::ref( s ) );
		std::thread t3( stest::thread_counter, std::ref( s ) );
		t3.join();
		s.signal( 2 );
		t1.join();
		t2.join();

		if ( stest::cnt == 10000 )
			test.success( "success" );
		else
			test.failure( "did not bump 10000 times" );
	};

	test["semaphore"] = [&]( void )
	{
		base::semaphore s;
		using stest = sem_test_struct<base::semaphore>;
		stest::cnt = int(0);
		std::thread t1( stest::thread_bump1, std::ref( s ) );
		std::thread t2( stest::thread_bump1, std::ref( s ) );
		std::thread t3( stest::thread_counter, std::ref( s ) );
		t3.join();
		s.signal( 2 );
		t1.join();
		t2.join();

		if ( stest::cnt == 10000 )
			test.success( "success" );
		else
			test.failure( "did not bump 10000 times" );
	};

	test["shared_mutex"] = [&]( void )
	{
		base::shared_mutex m;
		std::thread t1( &shared_reader, std::ref( m ) );
		std::thread t2( &shared_reader, std::ref( m ) );
		std::thread t3( &shared_writer, std::ref( m ) );
		t3.join();
		t2.join();
		t1.join();

		if ( shareCount == 10000 )
			test.success( "success" );
		else
			test.failure( "did not bump 10000 times" );
	};

	test["event"] = [&]( void )
	{
		base::event e1, e2;
		std::thread t1( &ev_1, std::ref( e1 ), std::ref( e2 ) );
		std::thread t2( &ev_2, std::ref( e1 ), std::ref( e2 ) );
		t2.join();
		t1.join();

		test.success( "success" );
	};

	test["auto_reset_event"] = [&]( void )
	{
		base::auto_reset_event e1, e2;
		std::thread t1( &auto_e1, std::ref( e1 ), std::ref( e2 ) );
		std::thread t2( &auto_e2, std::ref( e1 ), std::ref( e2 ) );
		t2.join();
		t1.join();

		test.success( "success" );
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
