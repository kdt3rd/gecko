
#include "tree_layout.h"

////////////////////////////////////////

tree_layout::tree_layout( const std::shared_ptr<container> &c, const std::shared_ptr<area> &a, double tab )
	: _tab( tab ), _container( c ), _tabbed( c, a, direction::RIGHT ), _layout( c )
{
	auto t = _tabbed.new_area( 0.0 );
	auto box = _tabbed.new_area( 1.0 );
	t->set_minimum( tab, 0 );

	_layout = box_layout( _container, box, direction::DOWN ); 
}

////////////////////////////////////////

std::shared_ptr<area> tree_layout::new_area( double w )
{
	return _layout.new_area( w );
}

////////////////////////////////////////

std::shared_ptr<tree_layout> tree_layout::new_branch( double w  )
{
	auto a = new_area( w );
	_children.push_back( std::make_shared<tree_layout>( _container, a, _tab ) );
	return _children.back();
}

////////////////////////////////////////

void tree_layout::recompute_minimum( void )
{
	for ( auto c: _children )
		c->recompute_minimum();
	_layout.recompute_minimum();
	_tabbed.recompute_minimum();
}

////////////////////////////////////////

void tree_layout::recompute_layout( void )
{
	_tabbed.recompute_layout();
	_layout.recompute_layout();
	for ( auto c: _children )
		c->recompute_layout();
}

////////////////////////////////////////

