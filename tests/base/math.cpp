// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/cmd_line.h>
#include <base/contract.h>
#include <base/half.h>
#include <base/ratio.h>
#include <base/string_util.h>
#include <base/unit_test.h>
#include <iostream>

////////////////////////////////////////

namespace
{
////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
    base::unit_test test( "math" );

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

    test["ratio"] = [&]( void ) {
        base::ratio oneT( 10, 100, true );
        if ( oneT.numerator() == 1 && oneT.denominator() == 10 )
            test.success( "ratio::simplify positive numbers" );
        else
            test.failure(
                "ratio::simplify should reduce 10/100 to 1/10, got {0}/{1}",
                oneT.numerator(),
                oneT.denominator() );

        base::ratio negV( -35, 49, true );
        if ( negV.numerator() == -5 && negV.denominator() == 7 )
            test.success( "ratio::simplify negative number" );
        else
            test.failure(
                "ratio::simplify should reduce -35/49 to -5/7, got {0}/{1}",
                negV.numerator(),
                negV.denominator() );

        std::stringstream tmp;
        tmp << oneT;
        if ( tmp.str() == "1/10" )
            test.success( "ratio operator<<" );
        else
            test.failure(
                "ratio operator<< should produce 1/10, got {0}", tmp.str() );

        base::ratio a( 24, 1 );
        base::ratio b( 1, 12 );
        base::ratio mab = a * b;
        if ( mab.numerator() == 2 && mab.denominator() == 1 )
            test.success( "ratio operator*" );
        else
            test.failure( "ratio operator* should produce 2/1, got {0}", mab );

        base::ratio fr1( 24, 1 );
        base::ratio fr2( 24000, 1001 );
        base::ratio nfr = fr1.common( fr2 );
        if ( nfr.numerator() == 24024 && nfr.denominator() == 1001 )
            test.success( "ratio common denominator" );
        else
            test.failure(
                "ratio common operator should produce 24024/1001, got {0}",
                nfr );

        base::ratio atest = fr1 + fr2;
        if ( atest.numerator() == 48024 && atest.denominator() == 1001 )
            test.success( "ratio addition" );
        else
            test.failure(
                "ratio addition operator should produce 48024/1001, got {0}",
                atest );
        base::ratio stest = fr1 - fr2;
        if ( stest.numerator() == 24 && stest.denominator() == 1001 )
            test.success( "ratio subtraction" );
        else
            test.failure(
                "ratio subtraction operator should produce 24/1001, got {0}",
                stest );
        base::ratio mtest = fr1 * fr2;
        if ( mtest.numerator() == 576000 && mtest.denominator() == 1001 )
            test.success( "ratio addition" );
        else
            test.failure(
                "ratio multiplication operator should produce 576000/1001, got {0}",
                mtest );
        base::ratio dtest = fr1 / fr2;
        if ( dtest.numerator() == 1001 && dtest.denominator() == 1000 )
            test.success( "ratio division" );
        else
            test.failure(
                "ratio division operator should produce 1001/1000, got {0}",
                dtest );
        if ( fr2.trunc() == 23 )
            test.success( "ratio truncation" );
        else
            test.failure(
                "ratio trunc operator should produce 23, got {0}",
                fr2.trunc() );
        if ( fr2.mod() == 977 )
            test.success( "ratio mod" );
        else
            test.failure(
                "ratio mod operator should produce 977, got {0}", fr2.mod() );
        base::ratio tfrc =
            86400 * ( base::ratio( 24, 1 ) / base::ratio( 48, 1 ) );
        if ( tfrc.round() == 43200 )
            test.success( "ratio round" );
        else
            test.failure(
                "ratio round operator should produce 43200, got {0} ({1})",
                tfrc.round(),
                tfrc );
    };

    test["half"] = [&]( void ) {
        using base::half;

        if ( sizeof( half ) == sizeof( uint16_t ) )
            test.success( "sizeof(half)" );
        else
            test.failure( "sizeof(half) expect same as uint16_t" );

        half y( base::half::binary, 0 );
        if ( y.bits() == 0 )
            test.success( "initialize half to 0" );
        else
            test.failure( "initialize half to 0, got {0}", y );

        half x( 1.F );
        if ( 1.F == static_cast<float>( x ) )
            test.success( "initialize half to 1.F" );
        else
            test.failure( "initialize half to 1.F, got {0}", x );

        x = 2.0;
        if ( 2.F == static_cast<float>( x ) )
            test.success( "assign half to double 2.0" );
        else
            test.failure( "assign half to double 2.0, got {0}", x );

        x = x / 2;
        if ( 1.F == static_cast<float>( x ) )
            test.success( "divide half" );
        else
            test.failure( "divide half(2.0)/2, got {0}", x );

        x = 3.141_h;
        if ( std::abs( static_cast<float>( x ) - 3.14062F ) < 0.00001F )
            test.success( "assign half to literal" );
        else
            test.success( "assign half to literal 3.141_h, got {0}", x );
    };

    //	test.cleanup() = [&]( void ) {};
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
