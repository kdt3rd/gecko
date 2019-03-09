// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

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

