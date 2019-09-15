// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/concurrent_vector.h>

#include <base/unit_test.h>

#include <iostream>
#ifndef _WIN32
#    include <signal.h>
#endif
#include <thread>

namespace
{

////////////////////////////////////////

void threadFunc( base::concurrent_vector<int> &x )
{
    try
    {
        for ( int i = 0; i < 1000; ++i )
            x.push_back( i );
    }
    catch ( const std::exception &e )
    {
        std::cerr << e.what() << '\n';
    }
}

////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
    base::unit_test test( "thread_ptr" );

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

    test["basic"] = [&]( void ) {
        base::concurrent_vector<int> v;
        if ( v.empty() )
            test.success( "construction and empty" );
        else
            test.failure( "call to empty fails" );

        v.push_back( 3 );

        if ( v.size() == 1 )
            test.success( "construction and push single thread test" );
        else
            test.failure( "unable to construct and add a single entry in a single thread" );

        if ( *(v.begin()) == 3 )
            test.success( "retrieval of first value ok" );
        else
            test.failure( "unable to retrieve first value, got {0}", *(v.begin()) );
    };
    test["concurrent"] = [&]( void ) {
        base::concurrent_vector<int> v;

        std::thread t1( threadFunc, std::ref( v ) );
        std::thread t2( threadFunc, std::ref( v ) );
        t1.join();
        t2.join();
        if ( v.size() == 2000 )
            test.success( "two threads pushing concurrently" );
        else
            test.failure( "unable to concurrently push only found {0} items", v.size() );
    };

    test.run( options );
    test.clean();

    return -static_cast<int>( test.failure_count() );
}

////////////////////////////////////////

} // namespace

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
