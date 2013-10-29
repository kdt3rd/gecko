
#pragma once

#include <memory>
#include "layout.h"
#include <core/area.h>
#include <core/container.h>
#include "tight_constraint.h"
#include "flow_constraint.h"

////////////////////////////////////////

class form_layout : public layout
{
public:
	form_layout( const std::shared_ptr<area> &a, direction dir = direction::RIGHT );

	size_t size( void ) const { return _areas.size(); }

	void set_direction( direction d ) { _columns.set_direction( d ); }

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> new_row( void );

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> get_row_areas( size_t i ) { return _areas.at( i ); }
	std::shared_ptr<area> get_row( size_t i ) { return _rows.at( i ).get_area( 0 ); }

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<area> _container;
	std::shared_ptr<area> _left_area;
	std::shared_ptr<area> _right_area;

	tight_constraint _left, _right;
	flow_constraint _down, _columns;
	std::vector<tight_constraint> _rows;

	std::vector<std::pair<std::shared_ptr<area>,std::shared_ptr<area>>> _areas;
};

////////////////////////////////////////

