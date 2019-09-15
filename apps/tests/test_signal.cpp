// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <base/signal.h>
#include <iostream>

namespace
{
int test1( int x )
{
    std::cout << "test1: " << x << std::endl;
    return 1;
}

class test2
{
public:
    int doit( int y )
    {
        std::cout << "test2::doit: " << y << std::endl;
        return 2;
    }
};

class sum_results
{
public:
    typedef int collector_result;

    int result( void ) { return _result; }

    bool operator()( int r )
    {
        _result += r;
        return true;
    }

private:
    int _result = 0;
};

int safemain( void )
{
    // Create a signal
    base::signal<int( int ), sum_results> sig;

    // Add a function callback
    sig.connect( test1 );

    // Add an object method callback
    test2 t;
    sig.connect( base::slot( t, &test2::doit ) );

    // Call the signal
    auto x = sig( 5 );

    // Print out the result
    std::cout << "signal sent: " << x << std::endl;

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
