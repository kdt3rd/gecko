
#pragma once

#include <memory>
#include "area.h"
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Box layout.
///
/// Organizes areas in single row (or column).
/// Each area can have a weight.  Extra space will be distributed among the areas by weight.
class box_layout : public layout
{
public:
	box_layout( direction dir = direction::RIGHT );

	void set_direction( direction d );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double horiz, double vert );

	void add( const std::shared_ptr<area> &a, double weight = 0.0 );

	virtual void recompute_minimum( area &a );
	virtual void recompute_layout( area &a );

private:
	flow_constraint _flow;
	tight_constraint _cross;
};

////////////////////////////////////////

}

