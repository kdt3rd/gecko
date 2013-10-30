
#pragma once

#include <core/contract.h>
#include <draw/area.h>
#include <memory>
#include <iostream>

namespace layout
{

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
	constraint( const std::shared_ptr<draw::area> &a )
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
	std::shared_ptr<draw::area> _area;
};

////////////////////////////////////////

}
