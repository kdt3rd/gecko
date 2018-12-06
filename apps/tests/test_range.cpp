//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/intervals.h>
#include <sstream>
#include <iostream>

#include <unistd.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	base::intervals<int> frames;

	frames.insert( 1, 15, 3, 6, 14, 4, 16, 5, 13, 250, 251, 252 );

	for ( auto &r: frames )
		std::cout << r << std::endl;

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
		base::print_exception( std::cerr, e );
	}
}
