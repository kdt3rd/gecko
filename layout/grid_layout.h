
#pragma once

#include <memory>
#include "area.h"
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
	grid_layout( void );

	std::vector<std::shared_ptr<area>> new_row( double w = 0.0 );
	std::vector<std::shared_ptr<area>> new_column( double w = 0.0 );

	void add_row( const std::vector<std::shared_ptr<area>> &add, double w );
	void add_column( const std::vector<std::shared_ptr<area>> &add, double w );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double horiz, double vert );

protected:
	void recompute_minimum( area &m );
	void recompute_layout( area &m );

private:
	std::shared_ptr<area> _container;

	struct group
	{
		group( void )
			: a( std::make_shared<area>() )
		{
		}

		std::shared_ptr<area> a;
		flow_constraint box;
		std::vector<std::pair<std::shared_ptr<tight_constraint>,std::shared_ptr<area>>> cells;
	};

	group _rows;
	group _columns;
};

////////////////////////////////////////

}

