
#pragma once

#include <vector>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

class flow_constraint : public constraint
{
public:
	flow_constraint( const std::shared_ptr<draw::area> &a, direction d = direction::RIGHT );

	void add_area( const std::shared_ptr<draw::area> &a, double weight = 0.0 );
	void remove_area( size_t i );

	void set_direction( direction d ) { _dir = d; }

	void recompute_minimum( void );
	void recompute_constraint( void );

private:
	direction _dir;
	std::vector<std::pair<std::shared_ptr<draw::area>,double>> _areas;
};

////////////////////////////////////////

}

