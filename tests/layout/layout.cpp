// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <base/unit_test.h>
#include <layout/layout.h>
#include <iostream>
#include <bitset>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "layout" );

	base::cmd_line options( argv[0] );
	test.setup( options );

	options.add_help();
	options.parse( argc, argv );

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return -1;
	}

	test["simple"] = [&]( void )
	{
		layout::layout l;

		layout::area w1( "w1" );
		layout::area w2( "w2" );

		l << ( w1.left() == 0.0 );
		l << ( w2.right() == 100.0 );
		l << ( w1.right() == w2.left() );
		l << ( w1.width() * 2 == w2.width() );

		l.update();
		std::cout << "W1 = " << w1 << '\n';
		std::cout << "W2 = " << w2 << '\n';
	};

	test.run( options );

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		std::cerr << '\n';
		base::print_exception( std::cerr, e );
	}
}
