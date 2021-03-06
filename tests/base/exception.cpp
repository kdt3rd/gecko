// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/cmd_line.h>
#include <base/contract.h>
#include <base/scope_guard.h>
#include <base/unit_test.h>
#include <base/compiler_support.h>
#include <fstream>

namespace
{
[[noreturn]] void bad_function( void )
{
    try
    {
        throw_runtime( "wrong side of the tracks" );
    }
    catch ( ... )
    {
        throw_add_location( "caught on the wrong side of the tracks" );
    }
}

[[noreturn]] void tryit( void )
{
    try
    {
        bad_function();
    }
    catch ( ... )
    {
        throw_add( "tried it, didn't work" );
    }
}

int safemain( int argc, char *argv[] )
{
    base::cmd_line options( argv[0] );

    base::unit_test test( "exception" );
    test.setup( options );

    auto errhandler =
        base::make_guard( [&]() { std::cerr << options << std::endl; } );
    options.add_help();
    options.parse( argc, argv );
    errhandler.dismiss();

    test["exception"] = [&]( void ) {
        try
        {
            throw_runtime( "fake exception" );
        }
        catch ( std::exception &e )
        {
            test.success( "caught exception ({0})", e.what() );
        }
    };

    test["location"] = [&]( void ) {
        try
        {
            tryit();
            GK_IGNORE_WARNING_BEGIN
            GK_IGNORE_WARNINGS
            test.failure( "tried and didn't failed" );
            GK_IGNORE_WARNING_END
        }
        catch ( std::exception &e )
        {
            std::stringstream msg;
            base::print_exception( msg, e );
            std::vector<std::string> lines;
            base::split( lines, msg.str(), '\n' );
            for ( std::string &n: lines )
                test.message( n );
            test.success( "tried and failed" );
        }
    };

    test.run( options );
    test.clean();

    return -static_cast<int>( test.failure_count() );
}

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
