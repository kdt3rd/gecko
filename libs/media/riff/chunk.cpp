//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "chunk.h"

namespace media
{
namespace riff
{

////////////////////////////////////////

chunk::chunk( std::istream &in )
{
	in >> _id;

	uint32_t s;
	in.read( reinterpret_cast<char*>( &s ), 4 );
	_size = s;

	_data = in.tellg();
}

////////////////////////////////////////

}
}

