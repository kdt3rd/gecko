
#include "container.h"
#include <core/reverse.h>

namespace gui
{

////////////////////////////////////////

container::container( const std::shared_ptr<draw::area> &a )
	: _area( a )
{
}

////////////////////////////////////////

container::~container( void )
{
}

////////////////////////////////////////

void container::add_view( const std::shared_ptr<::view::view> &v )
{
	v->set_delegate( get_delegate() );
	_views.push_back( v );
}

////////////////////////////////////////

void container::layout( const std::shared_ptr<draw::canvas> &c )
{
	for ( auto v: reverse( _views ) )
		v->layout( c );
	recompute_minimum();
	recompute_layout();
}

////////////////////////////////////////

void container::paint( const std::shared_ptr<draw::canvas> &c )
{
	for ( auto v: _views )
		v->paint( c );
}

////////////////////////////////////////

void container::recompute_minimum( void )
{
	for ( auto l: reverse( _layouts ) )
		l->recompute_minimum();
}

////////////////////////////////////////

void container::recompute_layout( void )
{
	for ( auto l: _layouts )
		l->recompute_layout();
}

////////////////////////////////////////

bool container::mouse_press( const draw::point &p, int button )
{
	for ( auto r: _reactors )
	{
		if ( r->mouse_press( p, button ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

bool container::mouse_release( const draw::point &p, int button )
{
	for ( auto r: _reactors )
	{
		if ( r->mouse_release( p, button ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

bool container::mouse_move( const draw::point &p )
{
	for ( auto r: _reactors )
	{
		if ( r->mouse_move( p ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

}

