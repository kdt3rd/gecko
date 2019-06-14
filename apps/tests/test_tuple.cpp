// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <base/tuple_util.h>
#include <iostream>
#include <sstream>

namespace
{
void testit( int x, float y ) { std::cout << x << ' ' << y << std::endl; }

void testagain( float x ) { std::cout << x << std::endl; }

int safemain( void )
{
    auto t = std::make_tuple( 42, 3.141592F );
    base::apply( testit, t );

    base::apply( testagain, base::tuple_tail( t ) );
    return 0;
}

} // namespace

int main( void )
{
    try
    {
        return safemain();
    }
    catch ( const std::exception &e )
    {
        base::print_exception( std::cerr, e );
    }
}
