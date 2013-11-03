
#pragma once

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

	virtual void recompute_minimum( void ) = 0;
	virtual void recompute_layout( void ) = 0;
};

////////////////////////////////////////

}

