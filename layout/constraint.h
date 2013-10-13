
#pragma once

#include <core/area.h>
#include <core/contract.h>
#include <memory>
#include <iostream>

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
		precondition( bool(_area), "missing area" );
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

