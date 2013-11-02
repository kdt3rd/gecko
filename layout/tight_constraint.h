
#pragma once

#include <vector>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

class tight_constraint : public constraint
{
public:
	tight_constraint( const std::shared_ptr<draw::area> &a, orientation o = orientation::VERTICAL );
	virtual ~tight_constraint( void )
	{
	}

	void set_orientation( orientation o ) { _orient = o; }

	void set_pad( double start, double end ) { _pad = { start, end }; }

	void add_area( const std::shared_ptr<draw::area> &a );
	std::shared_ptr<draw::area> get_area( size_t i ) { return _areas.at( i ); }

	void recompute_minimum( void );
	void recompute_constraint( void );

private:
	orientation _orient = orientation::VERTICAL;
	std::pair<double,double> _pad = { 0.0, 0.0 };
	std::vector<std::shared_ptr<draw::area>> _areas;
};

////////////////////////////////////////

}

