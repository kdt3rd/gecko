
#include "tree_layout.h"

namespace layout
{

////////////////////////////////////////

tree_layout::tree_layout( double tab )
{
	_box = std::make_shared<area>();
	_tab = std::make_shared<area>();
	_tab->set_minimum( tab, 0 );

	_tabbed.add( _tab );
	_tabbed.add( _box );

	_layout = box_layout( direction::DOWN ); 
}

////////////////////////////////////////

void tree_layout::set_pad( double left, double right, double top, double bottom )
{
	_layout.set_pad( left, right, top, bottom );
}

////////////////////////////////////////

void tree_layout::set_spacing( double s )
{
	_spacing = s;
	_layout.set_spacing( s, s );
	for ( auto c: _children )
		c.tree->set_spacing( s );
}

////////////////////////////////////////

void tree_layout::add( const std::shared_ptr<area> &a, double w )
{
	_layout.add( a, w );
}

////////////////////////////////////////

std::shared_ptr<tree_layout> tree_layout::new_branch( double w  )
{
	auto l = std::make_shared<tree_layout>( _tab->minimum_width() );
	l->set_spacing( _spacing );
	_children.push_back( l );
	add( _children.back().a );
	return l;
}

////////////////////////////////////////

void tree_layout::recompute_minimum( area &master )
{
	for ( auto c: _children )
		c.recompute_minimum();
	_layout.recompute_minimum( *_box );
	_tabbed.recompute_minimum( master );
}

////////////////////////////////////////

void tree_layout::recompute_layout( area &master )
{
	_tabbed.recompute_layout( master );
	_layout.recompute_layout( *_box );
	for ( auto c: _children )
		c.recompute_layout();
}

////////////////////////////////////////

}

