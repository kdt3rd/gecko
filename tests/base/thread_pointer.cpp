// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <base/thread_memory.h>
#include <base/unit_test.h>
#include <iostream>
#ifndef _WIN32
#    include <signal.h>
#endif
#include <atomic>
#include <thread>

namespace
{
static std::atomic<int> tdCount{ 0 };
static std::atomic<int> tdErrCount{ 0 };

struct DefThreadData
{
    DefThreadData( void ) { ++tdCount; }
    ~DefThreadData( void ) { --tdCount; }
    int dummy = 1;
};

struct CustomThreadData
{
    constexpr CustomThreadData( void ) noexcept = default;
    DefThreadData *operator()( void ) const { return new DefThreadData; }
};

struct ThreadData
{
    ThreadData( void )               = delete;
    ThreadData( const ThreadData & ) = default;
    ThreadData &operator=( const ThreadData & ) = default;

    explicit ThreadData( const char *ptr ) : _ptr( ptr ) { ++tdCount; }
    ~ThreadData( void ) { --tdCount; }

    const char *_ptr;
    int         dummy = 1;
};

using DefThreadDataPtr = base::thread_ptr<
    DefThreadData,
    std::default_delete<DefThreadData>,
    CustomThreadData>;

void defthread1( const DefThreadDataPtr &td ) { td->dummy = 2; }

void defthread2( const DefThreadDataPtr &td ) { td->dummy = 3; }

using CtorDefThreadDataPtr = base::thread_ptr<
    DefThreadData,
    std::default_delete<DefThreadData>,
    CustomThreadData>;

void ctorthread1( const CtorDefThreadDataPtr &td ) { td->dummy = 2; }

void ctorthread2( const CtorDefThreadDataPtr &td ) { td->dummy = 3; }

using ThreadDataPtr = base::thread_ptr<
    ThreadData,
    std::default_delete<ThreadData>,
    base::thread_specific_null_ctor<ThreadData>>;

void thread1( const ThreadDataPtr &td )
{
    if ( td.get() )
        ++tdErrCount;
    td.reset( new ThreadData( "Thread1" ) );
    td->dummy = 2;
}

void thread2( const ThreadDataPtr &td )
{
    if ( td.get() )
        ++tdErrCount;
    td.reset( new ThreadData( "Thread2" ) );
    td->dummy = 2;
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
        {
            DefThreadDataPtr p{ new DefThreadData };
            std::thread      t1( defthread1, std::cref( p ) );
            std::thread      t2( defthread2, std::cref( p ) );
            t1.join();
            t2.join();
        }
        if ( tdCount == 0 )
            test.success( "thread_ptr test" );
        else
            test.failure( "did not destroy data" );
    };

    test["custom_ctor"] = [&]( void ) {
        {
            CtorDefThreadDataPtr p;

            std::thread t1( ctorthread1, std::cref( p ) );
            std::thread t2( ctorthread2, std::cref( p ) );
            t1.join();
            t2.join();
        }
        if ( tdCount == 0 )
            test.success( "thread_ptr test" );
        else
            test.failure( "did not destroy data" );
    };

    test["null_ctor"] = [&]( void ) {
        {
            ThreadDataPtr p;

            std::thread t1( thread1, std::cref( p ) );
            std::thread t2( thread2, std::cref( p ) );
            t1.join();
            t2.join();
        }
        if ( tdCount == 0 && tdErrCount == 0 )
            test.success( "success" );
        else if ( tdErrCount != 0 )
            test.failure( "found unexpected data" );
        else
            test.failure( "did not destroy data" );
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
