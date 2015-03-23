
#include <base/math_functions.h>
#include <base/ratio.h>
#include <base/contract.h>
#include <sstream>
#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	std::cout << base::ratio( 10, 100 ) << std::endl;
	std::cout << base::ratio( -35, 49 ) << std::endl;
	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv);
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
