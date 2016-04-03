//
// Copyright (c) 2016 Kimball Thurston and Ian Godin
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

