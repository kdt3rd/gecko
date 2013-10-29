
#pragma once

#include <memory>
#include <core/area.h>
#include <core/container.h>
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

////////////////////////////////////////

class box_layout : public layout
{
public:
	box_layout( const std::shared_ptr<area> &a, direction dir = direction::RIGHT );

	void set_direction( direction d );

	size_t size( void ) const { return _areas.size(); }

	std::shared_ptr<area> new_area( double weight = 0.0 );

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

