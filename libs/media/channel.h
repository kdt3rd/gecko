//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>

namespace media
{

////////////////////////////////////////

class channel
{
public:
	channel( const std::string &n )
		: _name( n )
	{
	}

private:
	std::string _name;
};

////////////////////////////////////////

}

