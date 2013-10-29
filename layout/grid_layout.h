
#pragma once

#include <memory>
#include <core/area.h>
#include <core/container.h>
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

////////////////////////////////////////

class grid_layout : public layout
{
public:
	grid_layout( const std::shared_ptr<area> &c );

	std::vector<std::shared_ptr<area>> new_row( double w = 0.0 );
	std::vector<std::shared_ptr<area>> new_column( double w = 0.0 );

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<area> _container;

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

