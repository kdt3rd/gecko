
#pragma once

namespace platform
{

////////////////////////////////////////

class points
{
public:
	points( void ) {};
	virtual ~points( void ) {};

	virtual void add_point( double x, double y ) = 0;
};

////////////////////////////////////////

}

