
#pragma once

#include <memory>
#include "area.h"
#include "container.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

////////////////////////////////////////

class form_layout
{
public:
	form_layout( const std::shared_ptr<container> &c )
		: form_layout( c, c->bounds() )
	{
	}

	form_layout( const std::shared_ptr<container> &c, const std::shared_ptr<area> &a );

	size_t size( void ) const { return _areas.size(); }

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> new_line( void );

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> get_line( size_t i ) { return _areas.at( i ); }

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	std::shared_ptr<container> _container;
	std::shared_ptr<area> _left_area;
	std::shared_ptr<area> _right_area;

	tight_constraint _left, _right;
	flow_constraint _down, _columns;
	std::vector<tight_constraint> _rows;

	std::vector<std::pair<std::shared_ptr<area>,std::shared_ptr<area>>> _areas;
};

////////////////////////////////////////

