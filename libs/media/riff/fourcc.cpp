//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "fourcc.h"
#include <base/contract.h>

namespace media
{

////////////////////////////////////////

fourcc::fourcc( std::istream &in )
{
	in.read( _char, 4 );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const fourcc &c )
{
	out << c[0] << c[1] << c[2] << c[3];
	return out;
}

////////////////////////////////////////

std::istream &operator>>( std::istream &in, fourcc &c )
{
	uint32_t code;
	if ( in.read( reinterpret_cast<char *>( &code ), sizeof(uint32_t) ) )
		c.set( code );

	return in;
}

////////////////////////////////////////

}

