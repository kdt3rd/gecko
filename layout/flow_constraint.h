
#pragma once

#include <vector>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Flow areas in a direction.
class flow_constraint : public constraint
{
public:
	flow_constraint( const std::shared_ptr<area> &a, direction d = direction::RIGHT );

	void add_area( const std::shared_ptr<area> &a, double weight = 0.0 );
	void remove_area( size_t i );

	void set_direction( direction d ) { _dir = d; }
	direction get_direction( void ) const { return _dir; }

	void set_spacing( double s ) { _spacing = s; }
	void set_pad( double start, double end ) { _pad = { start, end }; }

	void recompute_minimum( void );
	void recompute_constraint( void );

private:
	direction _dir = direction::RIGHT;
	double _spacing = 0.0;
	std::pair<double,double> _pad = { 0.0, 0.0 };
	std::vector<std::pair<std::shared_ptr<area>,double>> _areas;
};

////////////////////////////////////////

}

