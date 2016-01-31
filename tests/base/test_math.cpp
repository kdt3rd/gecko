//
// Copyright (c) 2015 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <base/contract.h>
#include <base/string_util.h>
#include <base/unit_test.h>
#include <base/cmd_line.h>
#include <base/half.h>
#include <base/ratio.h>
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

	test["ratio"] = [&]( void )
	{
		base::ratio oneT( 10, 100 );
		if ( oneT.numerator() == 1 && oneT.denominator() == 10 )
			test.success( "ratio::simplify positive numbers" );
		else
			test.failure( "ratio::simplify should reduce 10/100 to 1/10, got {0}/{1}", oneT.numerator(), oneT.denominator() );

		base::ratio negV( -35, 49 );
		if ( negV.numerator() == -5 && negV.denominator() == 7 )
			test.success( "ratio::simplify negative number" );
		else
			test.failure( "ratio::simplify should reduce -35/49 to -5/7, got {0}/{1}", negV.numerator(), negV.denominator() );

		std::stringstream tmp;
		tmp << oneT;
		if ( tmp.str() == "1/10" )
			test.success( "ratio operator<<" );
		else
			test.failure( "ratio operator<< should produce 1/10, got {0}", tmp.str() );
	};

	test["half"] = [&]( void )
	{
		using base::half;
		using namespace base::literal;

		if ( sizeof(half) == sizeof(uint16_t) )
			test.success( "sizeof(half)" );
		else
			test.failure( "sizeof(half) expect same as uint16_t" );

		half y( base::half::binary, 0 );
		if ( y.bits() == 0 )
			test.success( "initialize half to 0" );
		else
			test.failure( "initialize half to 0, got {0}", y );

		half x( 1.F );
		if ( 1.F == static_cast<float>(x) )
			test.success( "initialize half to 1.F" );
		else
			test.failure( "initialize half to 1.F, got {0}", x );

		x = 2.0;
		if ( 2.F == static_cast<float>(x) )
			test.success( "assign half to double 2.0" );
		else
			test.failure( "assign half to double 2.0, got {0}", x );

		x = x / 2;
		if ( 1.F == static_cast<float>(x) )
			test.success( "divide half" );
		else
			test.failure( "divide half(2.0)/2, got {0}", x );

		x = 3.141_h;
		if ( std::abs( static_cast<float>(x) - 3.14062 ) < 0.00001 )
			test.success( "assign half to literal" );
		else
			test.success( "assign half to literal 3.141_h, got {0}", x );
	};
	
//	test.cleanup() = [&]( void ) {};
	test.run( options );
	test.clean();
	
	return - static_cast<int>( test.failure_count() );
}

} // empty namespace

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



