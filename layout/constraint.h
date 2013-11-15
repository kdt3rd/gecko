
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
	constraint( void )
	{
	}

	virtual ~constraint( void )
	{
	}

	virtual void recompute_minimum( area &a ) = 0;
	virtual void recompute_constraint( area &a ) = 0;
};

////////////////////////////////////////

}
