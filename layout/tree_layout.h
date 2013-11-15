
#pragma once

#include <memory>
#include "layout.h"
#include "box_layout.h"
#include "simple_area.h"

namespace layout
{

////////////////////////////////////////

/// @brief Tree layout.
///
/// Layout 
template<typename container, typename area>
class tree_layout : public layout<container, area>
{
public:
	tree_layout( double tab = 12.0 )
	{
		_box = std::make_shared<simple_area>();
		_tab = std::make_shared<simple_area>();
		_tab->set_minimum( tab, 0 );

		_tabbed.add( _tab );
		_tabbed.add( _box );

		_layout.set_direction( direction::DOWN );
	}

	virtual void set_pad( double left, double right, double top, double bottom )
	{
		_layout.set_pad( left, right, top, bottom );
	}

	virtual void set_spacing( double horiz, double vert )
	{
		_spacing = horiz;
		_layout.set_spacing( horiz, vert );
		for ( auto c: _children )
			c.tree->set_spacing( horiz, vert );
	}

	void add( const std::shared_ptr<area> &a, double w = 0.0 )
	{
		_layout.add( a, w );
	}

	std::shared_ptr<tree_layout> new_branch( double w = 0.0 )
	{
		auto l = std::make_shared<tree_layout>( _tab->minimum_width() );
		l->set_spacing( _spacing, _spacing );
//		_children.push_back( l );
//		add( _children.back().a );
		return l;
	}

	virtual void recompute_minimum( container &master )
	{
		for ( auto c: _children )
			c.recompute_minimum();
		_layout.recompute_minimum( *_box );
		_tabbed.recompute_minimum( master );
	}

	virtual void recompute_layout( container &master )
	{
		_tabbed.recompute_layout( master );
		_layout.recompute_layout( *_box );
		for ( auto c: _children )
			c.recompute_layout();
	}

private:
	std::shared_ptr<simple_area> _tab;
	std::shared_ptr<simple_area> _box;
	double _spacing = 0.0;
	box_layout<container,simple_area> _tabbed;
	box_layout<simple_area,area> _layout;

	struct child
	{
		child( const std::shared_ptr<tree_layout<simple_area,area>> &t )
			: tree( t ), a( std::make_shared<simple_area>() )
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

		std::shared_ptr<tree_layout<simple_area,area>> tree;
		std::shared_ptr<simple_area> a;
	};

	std::vector<child> _children;
};

////////////////////////////////////////

}

