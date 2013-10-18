
#pragma once

#include <platform/points.h>

namespace xcb
{

////////////////////////////////////////

class points : public platform::points
{
public:
	points( void ) {};
	virtual ~points( void ) {};

	virtual void add_point( double x, double y )
	{
		xcb_point_t p = { int16_t(x), int16_t(y) };
		_points.push_back( p );
	}

	const std::vector<xcb_point_t> &get_list( void ) const { return _points; }

private:
	std::vector<xcb_point_t> _points;
};

////////////////////////////////////////

}

