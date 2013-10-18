
#pragma once

#include <platform/rectangles.h>

namespace xcb
{

////////////////////////////////////////

class rectangles : public platform::rectangles
{
public:
	rectangles( void ) {};
	virtual ~rectangles( void ) {};

	virtual void add_rectangle( double x, double y, double w, double h )
	{
		xcb_rectangle_t r = { int16_t(x), int16_t(y), uint16_t(w), uint16_t(h) };
		_rects.push_back( r );
	}

	const std::vector<xcb_rectangle_t> &get_list( void ) const { return _rects; }

private:
	std::vector<xcb_rectangle_t> _rects;

};

////////////////////////////////////////

}

