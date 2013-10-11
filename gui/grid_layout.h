
#pragma once

#include <memory>
#include "area.h"
#include "container.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

////////////////////////////////////////

class grid_layout
{
public:
	grid_layout( const std::shared_ptr<container> &c )
		: grid_layout( c, c->bounds() )
	{
	}

	grid_layout( const std::shared_ptr<container> &c, const std::shared_ptr<area> &a );

	void new_row( double w = 0.0 );
	void new_column( double w = 0.0 );

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<container> _container;

	struct group
	{
		group( const std::shared_ptr<area> &a )
			: box( a )
		{
		}

		flow_constraint box;
		std::vector<tight_constraint> cells;
	};

	group _rows;
	group _columns;
};

////////////////////////////////////////

