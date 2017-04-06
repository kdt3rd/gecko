//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "contract.h"
#include <iostream>

namespace base
{

location_exception::location_exception( const char *file, int line )
	: _line( line ), _file( file )
{
	std::ostringstream str;
	str << "file " << file << " line " << line;
	_msg = str.str();
}

////////////////////////////////////////

location_exception::~location_exception( void )
{
}


////////////////////////////////////////

precondition_error::~precondition_error( void )
{
}


////////////////////////////////////////


postcondition_error::~postcondition_error( void )
{
}


////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level )
{
	if ( level == 0 )
		out << "ERROR:\n";
	out << "  " << e.what() << '\n';
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
	if ( level == 0 )
		out << std::flush;
}

////////////////////////////////////////

}

