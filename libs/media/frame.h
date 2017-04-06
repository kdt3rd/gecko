//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "channel.h"
#include <cstdint>
#include <memory>

namespace media
{

////////////////////////////////////////

class frame
{
public:
	frame( int64_t fn )
		: _number( fn )
	{
	}

	int64_t number( void ) const
	{
		return _number;
	}

	size_t size( void ) const;

	std::shared_ptr<channel> operator[]( size_t i );
	std::shared_ptr<channel> at( size_t i );

private:
	int64_t _number;
};

////////////////////////////////////////

}

