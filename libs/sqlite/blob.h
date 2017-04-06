//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdlib>

namespace sqlite
{

////////////////////////////////////////

class blob
{
public:
	blob( const void *d, size_t s )
		: _data( d ), _size( s )
	{
	}

	const void *data( void ) const { return _data; }
	size_t size( void ) const { return _size; }

private:
	const void *_data = nullptr;
	size_t _size = 0;
};

////////////////////////////////////////

}
