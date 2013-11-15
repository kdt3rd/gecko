
#pragma once

#include <memory>
#include "layout.h"
#include "area.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Form layout.
///
/// Lays out areas in 2 columns, one for labels, another for widgets.
class form_layout : public layout
{
public:
	form_layout( direction dir = direction::RIGHT );

	size_t size( void ) const { return _areas.size(); }

	void set_direction( direction d ) { _columns.set_direction( d ); }

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double horiz, double vert );

//	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> new_row( void );
	void add( const std::shared_ptr<area> &l, const std::shared_ptr<area> &a );

//	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> get_row_areas( size_t i ) { return _areas.at( i ); }
//	std::shared_ptr<area> get_row( size_t i ) { return _rows.at( i ).get_area( 0 ); }

protected:
	void recompute_minimum( area &a );
	void recompute_layout( area &a );

private:
	std::shared_ptr<area> _left_area;
	std::shared_ptr<area> _right_area;

	tight_constraint _left, _right;
	flow_constraint _down, _columns;
	std::vector<std::pair<tight_constraint,area>> _rows;

	std::vector<std::pair<std::shared_ptr<area>,std::shared_ptr<area>>> _areas;
};

////////////////////////////////////////

}

