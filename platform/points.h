
#pragma once

namespace platform
{

////////////////////////////////////////

class points
{
public:
	points( void ) {};
	virtual ~points( void ) {};

	void add_point( const std::pair<double,double> &p ) { add_point( p.first, p.second ); }
	virtual void add_point( double x, double y ) = 0;
};

////////////////////////////////////////

}

