
#pragma once

#include "area.h"
#include <memory>

////////////////////////////////////////

enum class direction
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

////////////////////////////////////////

enum class orientation
{
	HORIZONTAL,
	VERTICAL,
};

////////////////////////////////////////

class constraint
{
public:
	constraint( const std::shared_ptr<area> &a )
		: _area( a )
	{
	}

	virtual ~constraint( void )
	{
	}

	virtual void recompute_minimum( void ) = 0;
	virtual void recompute_constraint( void ) = 0;

protected:
	std::shared_ptr<area> _area;
};

////////////////////////////////////////

