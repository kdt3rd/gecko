
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

