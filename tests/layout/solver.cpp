//
// Copyright (c) 2016-2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <base/contract.h>
#include <base/unit_test.h>
#include <layout/solver.h>
#include <iostream>
#include <bitset>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "solver" );

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
		layout::solver s;
		layout::variable x0( "x0" );
		layout::variable x1( "x1" );

		s.add_variable( x1, 1.0 );
		s.add_constraint( x0 >= 0 );
		s.add_constraint( x1 >= 0 );
		s.add_constraint( ( x0 * 2 ) == x1 );
		s.suggest( x1, 1 );

		s.update_variables();

		std::cout << x0 << " = " << x0.value() << '\n';
		std::cout << x1 << " = " << x1.value() << '\n';
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
