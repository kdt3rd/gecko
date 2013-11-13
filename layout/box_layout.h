
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
	box_layout( const std::shared_ptr<area> &a, direction dir = direction::RIGHT );

	void set_direction( direction d );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double horiz, double vert );

	size_t size( void ) const { return _areas.size(); }

	std::shared_ptr<area> new_area( double weight = 0.0 );
	void add_area( const std::shared_ptr<area> &a, double weight = 0.0 );

	std::shared_ptr<area> get_area( size_t i ) { return _areas.at( i ); }

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<area> _container;

	flow_constraint _flow;
	tight_constraint _cross;

	std::vector<std::shared_ptr<area>> _areas;
};

////////////////////////////////////////

}

