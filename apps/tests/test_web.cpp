// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/cmd_line.h>
#include <base/contract.h>
#include <base/scope_guard.h>
#include <base/uri.h>
#include <web/client.h>

namespace
{
int safemain( int argc, char *argv[] )
{
    base::cmd_line options(
        argv[0],
        base::cmd_line::option(
            0,
            "",
            "<uri> ...",
            base::cmd_line::args,
            "List of URI to connect to",
            true ) );

    auto errhandler =
        base::make_guard( [&]() { std::cerr << options << std::endl; } );
    options.parse( argc, argv );
    errhandler.dismiss();

    if ( auto &opt = options["<uri> ..."] )
    {
        web::client test( "tester" );
        for ( auto &u: opt.values() )
        {
            base::uri     path( u );
            web::response resp = test.get( path );
            std::cout << static_cast<int>( resp.status() ) << ' '
                      << resp.reason() << "\n"
                      << resp.content() << std::endl;
        }
    }

    return 0;
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
}
