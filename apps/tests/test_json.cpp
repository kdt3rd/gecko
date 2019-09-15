// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <base/format.h>
#include <base/json.h>
#include <iostream>

namespace
{
int safemain( void )
{
    base::json value( 1.5 );

    base::json array(
        { base::json( 1.5 ), base::json( "Hello" ), base::json( true ) } );

    base::json object( { { "first", base::json() },
                         { "second", base::json( "Hello" ) },
                         { "third", base::json( false ) } } );

    base::json array2;
    array2.push_back( base::json( true ) );
    array2.push_back( base::json( false ) );

    base::json object2;
    object2["test"]  = base::json( "1 2 3" );
    object2["check"] = base::json( "1 2 3 4" );

    std::cout << value << std::endl;
    std::cout << array << std::endl;
    std::cout << array2 << std::endl;
    std::cout << object << std::endl;
    std::cout << object2 << std::endl;

    std::string test(
        "{ \"a\": 1.5, \"b\": [ 1, null, 3, \"hello\"], \"c\": true, \"d\": false, \"e\": [] }" );
    base::json doc;
    test >> doc;
    std::cout << doc << std::endl;
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
