
#pragma once

#include "area.h"
#include <list>
#include <memory>

namespace layout
{

////////////////////////////////////////

/// @brief A layout
///
class layout : public area
{
public:
	void set_padding( double l, double r, double t, double b )
	{
		_pad[0] = l;
		_pad[1] = r;
		_pad[2] = t;
		_pad[3] = b;
	}

	void set_spacing( double h, double v )
	{
		_spacing[0] = h;
		_spacing[1] = v;
	}

protected:
	void expand_width( const std::list<std::weak_ptr<area>> &areas, double extra );
	void expand_height( const std::list<std::weak_ptr<area>> &areas, double extra );

	double _pad[4] = { 0.0 };
	double _spacing[2] = { 0.0 };
};

////////////////////////////////////////

}

