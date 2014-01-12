
#include "tree_node.h"

namespace gui
{

////////////////////////////////////////

void tree_node::set_collapsed( bool c )
{
	if ( c != _collapsed )
	{
		_collapsed = c;
		invalidate();
	}
}

////////////////////////////////////////

void tree_node::compute_minimum( void )
{
	if ( _collapsed )
	{
		_root->compute_minimum();
		set_minimum( _root->minimum_width(), _root->minimum_height() );
	}
	else
		container<tree_layout>::compute_minimum();
}

////////////////////////////////////////

void tree_node::compute_layout( void )
{
	if ( _collapsed )
	{
		_root->set_position( position() );
		_root->set_size( width(), height() );
	}
	else
		container<tree_layout>::compute_layout();
}

////////////////////////////////////////

bool tree_node::mouse_press( const core::point &p, int button )
{
	if ( _collapsed )
	{
		if ( _root->mouse_press( p, button ) )
		{
			_mouse_grab = _root;
			return true;
		}
		return widget::mouse_press( p, button );
	}
	else
		return container<tree_layout>::mouse_press( p, button );
}

////////////////////////////////////////

bool tree_node::mouse_release( const core::point &p, int button )
{
	if ( _mouse_grab )
	{
		auto tmp = _mouse_grab;
		_mouse_grab.reset();
		return tmp->mouse_release( p, button );
	}

	if ( _collapsed )
		return _root->mouse_release( p, button );
	else
		return container<tree_layout>::mouse_release( p, button );
}

////////////////////////////////////////

bool tree_node::mouse_move( const core::point &p )
{
	if ( _mouse_grab )
		return _mouse_grab->mouse_move( p );

	if ( _collapsed )
		return _root->mouse_move( p );
	else
		return container<tree_layout>::mouse_move( p );
}

////////////////////////////////////////

void tree_node::paint( const std::shared_ptr<draw::canvas> &c )
{
	if ( _collapsed )
		_root->paint( c );
	else
		container<tree_layout>::paint( c );
}

////////////////////////////////////////

}
