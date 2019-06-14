// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/async_io.h>
#include <base/contract.h>
#include <base/file_system.h>
#include <base/scope_guard.h>
#include <base/unit_test.h>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#    include <base/win32_file_system.h>
#else
#    include <base/posix_file_system.h>
#endif

namespace
{
////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
    base::unit_test test( "units" );

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

    base::uri tmppath( "file", "", "tmp", "test_async_io" );
    auto      fs = base::file_system::get( tmppath );
    on_scope_exit { fs->unlink( tmppath ); };

    {
        base::ostream testoutput = fs->open_write( tmppath );
        testoutput << "Hello, world!" << std::endl;
    }

#ifdef __linux__
    test["linux_aio"] = [&]( void ) {
        base::async_io aiodriver;
        base::istream  testinput = fs->open_read( tmppath );
        char           buf[14];
        buf[13]            = '\0';
        int     idle_count = 0;
        ssize_t nread      = aiodriver.idle_read(
            [&]() { ++idle_count; }, buf, 13, testinput.rdbuf(), 0 );
        if ( nread == 13 && 0 == strcmp( buf, "Hello, world!" ) &&
             idle_count > 0 )
            test.success( "Able to read from tmp file" );
        else
            test.failure(
                "aio driver idled {0}, read '{1}' instead of 'Hello, world!'",
                idle_count,
                buf );
        idle_count = 0;
        nread      = aiodriver.idle_read(
            [&]() { ++idle_count; }, buf, 1, testinput.rdbuf(), 12 );
        buf[1] = '\0';
        if ( nread == 1 && buf[0] == '!' && idle_count > 0 )
            test.success( "Able to read from tmp file at offset" );
        else
            test.failure(
                "aio driver idled {0}, read '{1}' instead of '!'",
                idle_count,
                buf );
    };
#endif

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
