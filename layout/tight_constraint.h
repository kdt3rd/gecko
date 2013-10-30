
#pragma once

#include "constraint.h"

////////////////////////////////////////

class tight_constraint : public constraint
{
public:
	tight_constraint( const std::shared_ptr<draw::area> &a, orientation o = orientation::VERTICAL );
	virtual ~tight_constraint( void )
	{
	}

	void set_orientation( orientation o ) { _orient = o; }

	void add_area( const std::shared_ptr<draw::area> &a );
	std::shared_ptr<draw::area> get_area( size_t i ) { return _areas.at( i ); }

	void recompute_minimum( void );
	void recompute_constraint( void );

private:
	orientation _orient;
	std::vector<std::shared_ptr<draw::area>> _areas;
};

////////////////////////////////////////

