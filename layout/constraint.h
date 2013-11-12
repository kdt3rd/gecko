
#pragma once

#include <core/contract.h>
#include <core/direction.h>
#include <core/orientation.h>
#include <memory>
#include <iostream>
#include "area.h"

namespace layout
{

////////////////////////////////////////

/// @brief Layout constraint.
///
/// Constraints on a layout.
/// Constraints are generally one dimensional (either horizontal or vertical).
/// A layout is made of several constraints working together.
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

}
