
#include "contract.h"
#include <iostream>

namespace base
{

////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level )
{
	out << std::string( level, ' ' ) << "exception: " << e.what() << '\n';
	try
	{
		std::rethrow_if_nested( e );
	}
	catch ( std::exception &e )
	{
		print_exception( out, e, level + 1 );
	}
	catch ( ... )
	{
	}
}

////////////////////////////////////////

}

