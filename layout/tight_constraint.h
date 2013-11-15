
#pragma once

#include <vector>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Tight constraint.
///
/// Superimpose areas on top of each other.
class tight_constraint : public constraint
{
public:
	tight_constraint( orientation o = orientation::VERTICAL );
	virtual ~tight_constraint( void )
	{
	}

	void set_orientation( orientation o ) { _orient = o; }

	void set_pad( double start, double end ) { _pad = { start, end }; }

	void add_area( const std::shared_ptr<area> &a );
	std::shared_ptr<area> get_area( size_t i ) { return _areas.at( i ); }

	void recompute_minimum( area &a );
	void recompute_constraint( area &a );

private:
	orientation _orient = orientation::VERTICAL;
	std::pair<double,double> _pad = { 0.0, 0.0 };
	std::vector<std::shared_ptr<area>> _areas;
};

////////////////////////////////////////

}

