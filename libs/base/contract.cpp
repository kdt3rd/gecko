
#include "contract.h"
#include <iostream>

namespace base
{

////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level )
{
	out << std::string( level, ' ' ) << "error: " << e.what() << '\n';
	try
	{
		std::rethrow_if_nested( e );
	}
	catch ( std::exception &more )
	{
		print_exception( out, more, level + 1 );
	}
	catch ( ... )
	{
		out << "UNKNOWN EXCEPTION" << std::endl;
	}
}

////////////////////////////////////////

}

