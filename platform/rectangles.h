
#pragma once

namespace platform
{

////////////////////////////////////////

class rectangles
{
public:
	rectangles( void ) {};
	virtual ~rectangles( void ) {};

	virtual void add_rectangle( double x, double y, double w, double h ) = 0;
};

////////////////////////////////////////

}

