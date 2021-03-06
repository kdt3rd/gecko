// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <fstream>
#include <imgproc/expr_parser.h>
#include <imgproc/token.h>
#include <map>
#include <memory>

namespace
{
int safemain( int argc, char *argv[] )
{
    precondition( argc == 2, "expected single argument" );

    std::ifstream     src( argv[1] );
    imgproc::iterator token( src );

    imgproc::expr_parser parser( token );

    auto e = parser.expression();

    if ( token )
        std::cout << base::format(
                         "didn't parse everything: '{0}'", token.value() )
                  << std::endl;
    ;

    if ( e )
    {
        e->write( std::cout );
        std::cout << std::endl;
    }
    else
        throw_runtime( "null expression" );

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
