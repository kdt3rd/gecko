//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "fourcc.h"
#include <iostream>
#include <cstdint>
#include <base/contract.h>

namespace media
{
namespace riff
{

////////////////////////////////////////

class chunk
{
public:
	chunk( std::istream &in );

	const fourcc &id( void ) const
	{
		return _id;
	}

	size_t size( void ) const
	{
		return _size;
	}

	std::istream &seek( std::istream &in )
	{
		in.seekg( _data );
		return in;
	}

private:
	fourcc _id;
	size_t _size;
	std::istream::pos_type _data;
};

////////////////////////////////////////

}
}

