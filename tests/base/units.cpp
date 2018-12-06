//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#include <base/contract.h>
#include <base/unit_test.h>
#include <base/units.h>
#include <base/math_functions.h>

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

	test["literal_constructors"] = [&]( void )
	{
        using namespace base::units;
        using namespace base::literals::length_literals;
        auto x = 3000_mm;
        auto y = 3_m;

        if ( x == y )
            test.success( "success" );
        else
            test.success( "literal constructors _m and _mm not working" );
	};

	test["change_type"] = [&]( void )
	{
        using namespace base::units;
        auto x = 3000_mm;
        base::units::millimeters<float> y = x;
		if ( x.count() == static_cast<uint64_t>( y.count() ) )
			test.success( "success" );
		else
			test.failure( "conversion to int failed" );
	};

	test["complex_conversion"] = [&]( void )
	{
        using namespace base::units;
        auto x = 1_in;
        base::units::centimeters<float> y = x;
		if ( base::equal( y.count(), 2.54f ) )
			test.success( "success" );
		else
			test.failure( "conversion inches to cm failed" );
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
}

////////////////////////////////////////

}

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
