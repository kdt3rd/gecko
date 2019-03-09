// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/uri.h>
#include <base/contract.h>
#include <base/half.h>
#include <sstream>
#include <iostream>
#include <typeindex>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	std::cout << "std::type_info = " << sizeof(std::type_info) << std::endl;
	std::cout << "std::type_index = " << sizeof(std::type_index) << std::endl;
	std::cout << "base::half = " << sizeof(base::half) << std::endl;
	std::cout << "base::uri = " << sizeof(base::uri) << std::endl;
	std::cout << "unsigned long long = " << sizeof(unsigned long long) << std::endl;
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
