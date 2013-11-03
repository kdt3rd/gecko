
#pragma once

#include <memory>
#include "layout.h"
#include <draw/area.h>
#include "box_layout.h"

namespace layout
{

////////////////////////////////////////

/// @brief Tree layout.
///
/// Layout 
class tree_layout : public layout
{
public:
	tree_layout( const std::shared_ptr<draw::area> &c, double tab );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double s );

	std::shared_ptr<draw::area>  new_area( double w = 0.0 );
	std::shared_ptr<tree_layout> new_branch( double w = 0.0 );

	void recompute_minimum( void );
	void recompute_layout( void );

private:
	double _tab = 10.0;
	double _spacing = 0.0;
	std::shared_ptr<draw::area> _container;
	box_layout _tabbed;
	box_layout _layout;

	std::vector<std::shared_ptr<tree_layout>> _children;
};

////////////////////////////////////////

}

