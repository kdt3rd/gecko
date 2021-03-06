// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <base/half.h>
#include <base/uri.h>
#include <iostream>
#include <sstream>
#include <typeindex>

namespace
{
int safemain( int /*argc*/, char * /*argv*/[] )
{
    std::cout << "std::type_info = " << sizeof( std::type_info ) << std::endl;
    std::cout << "std::type_index = " << sizeof( std::type_index ) << std::endl;
    std::cout << "base::half = " << sizeof( base::half ) << std::endl;
    std::cout << "base::uri = " << sizeof( base::uri ) << std::endl;
    std::cout << "unsigned long long = " << sizeof( unsigned long long )
              << std::endl;
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
