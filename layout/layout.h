
#pragma once

#include "area.h"

namespace layout
{

////////////////////////////////////////

/// @brief A layout
///
/// Layouts work in 2 passes.
/// The first pass computes the minimum size from areas.
/// The second pass computes the actual position and size of each area.
class layout
{
public:
	virtual ~layout( void )
	{
	}

	virtual void recompute_minimum( area &a ) = 0;
	virtual void recompute_layout( area &a ) = 0;
};

////////////////////////////////////////

}

