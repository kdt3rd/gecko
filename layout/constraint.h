
#pragma once

#include <core/contract.h>
#include <core/direction.h>
#include <core/orientation.h>
#include <memory>
#include <iostream>

namespace layout
{

////////////////////////////////////////

/// @brief Layout constraint.
///
/// Constraints on a layout.
/// Constraints are generally one dimensional (either horizontal or vertical).
/// A layout is made of several constraints working together.
template<typename container>
class constraint
{
public:
	constraint( void )
	{
	}

	virtual ~constraint( void )
	{
	}

	virtual void recompute_minimum( container &a ) = 0;
	virtual void recompute_constraint( container &a ) = 0;
};

////////////////////////////////////////

}
