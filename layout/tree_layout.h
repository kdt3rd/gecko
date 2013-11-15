
#pragma once

#include <memory>
#include "layout.h"
#include "area.h"
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
	tree_layout( double tab = 12.0 );

	void set_pad( double left, double right, double top, double bottom );
	void set_spacing( double s );

	void add( const std::shared_ptr<area> &a, double w = 0.0 );

	std::shared_ptr<tree_layout> new_branch( double w = 0.0 );

	virtual void recompute_minimum( area &a );
	virtual void recompute_layout( area &a );

private:
	std::shared_ptr<area> _tab;
	std::shared_ptr<area> _box;
	double _spacing = 0.0;
	box_layout _tabbed;
	box_layout _layout;

	struct child
	{
		child( const std::shared_ptr<tree_layout> &t )
			: tree( t ), a( std::make_shared<area>() )
		{
		}

		void recompute_layout( void )
		{
			tree->recompute_layout( *a );
		}

		void recompute_minimum( void )
		{
			tree->recompute_minimum( *a );
		}

		std::shared_ptr<tree_layout> tree;
		std::shared_ptr<area> a;
	};

	std::vector<child> _children;
};

////////////////////////////////////////

}

