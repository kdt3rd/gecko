//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
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

