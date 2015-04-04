
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/half.h>
#include <iostream>

namespace
{

int safemain( void )
{
	using base::half;
	using namespace base::literal;

	half x( 1.0 );
	std::cout << "1.0 = " << x << std::endl;

	x = 2.0;
	std::cout << "2.0 = " << x << std::endl;

	x = x / 2;
	std::cout << "2.0 / 2 = " << x << std::endl;

	std::cout << "3.141 = " << 3.141_h << std::endl;

	return 0;
}

}

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
