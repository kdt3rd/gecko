
#include "tree_layout.h"

namespace layout
{

////////////////////////////////////////

tree_layout::tree_layout( const std::shared_ptr<area> &c, double tab )
	: _tab( tab ), _container( c ), _tabbed( c ), _layout( c )
{
	auto t = _tabbed.new_area( 0.0 );
	auto box = _tabbed.new_area( 1.0 );
	t->set_minimum( tab, 0 );

	_layout = box_layout( box, direction::DOWN ); 
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
		c->set_spacing( s );
}

////////////////////////////////////////

std::shared_ptr<area> tree_layout::new_area( double w )
{
	return _layout.new_area( w );
}

////////////////////////////////////////

void tree_layout::add_area( const std::shared_ptr<area> &a, double w )
{
	_layout.add_area( a, w );
}

////////////////////////////////////////

std::shared_ptr<tree_layout> tree_layout::new_branch( double w  )
{
	auto a = new_area( w );
	auto l = std::make_shared<tree_layout>( a, _tab );
	l->set_spacing( _spacing );
	_children.push_back( l );
	return l;
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

}

