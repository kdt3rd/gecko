
#pragma once

#include "constraint.h"

////////////////////////////////////////

class flow_constraint : public constraint
{
public:
	flow_constraint( const std::shared_ptr<area> &a, direction d );

	void add_area( const std::shared_ptr<area> &a, double weight = 0.0 );
	void remove_area( size_t i );

	void set_direction( direction d ) { _dir = d; }

	void recompute_minimum( void );
	void recompute_constraint( void );

private:
	direction _dir;
	std::vector<std::pair<std::shared_ptr<area>,double>> _areas;
};

////////////////////////////////////////

