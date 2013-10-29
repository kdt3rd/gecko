
#pragma once

#include <memory>
#include "layout.h"
#include <core/area.h>
#include <core/container.h>
#include "box_layout.h"

////////////////////////////////////////

class tree_layout : public layout
{
public:
	tree_layout( const std::shared_ptr<area> &c, double tab );

	std::shared_ptr<area>  new_area( double w = 0.0 );
	std::shared_ptr<tree_layout> new_branch( double w = 0.0 );

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	double _tab;
	std::shared_ptr<area> _container;
	box_layout _tabbed;
	box_layout _layout;

	std::vector<std::shared_ptr<tree_layout>> _children;
};

////////////////////////////////////////

