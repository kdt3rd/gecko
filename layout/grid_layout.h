
#pragma once

#include <memory>
#include <draw/area.h>
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Grid layout.
///
/// Lays out areas in a grid of rows and columns.
class grid_layout : public layout
{
public:
	grid_layout( const std::shared_ptr<draw::area> &c );

	std::vector<std::shared_ptr<draw::area>> new_row( double w = 0.0 );
	std::vector<std::shared_ptr<draw::area>> new_column( double w = 0.0 );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double horiz, double vert );

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<draw::area> _container;

	struct group
	{
		group( const std::shared_ptr<draw::area> &a )
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

}

