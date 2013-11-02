
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

bool container::mousePress( const draw::point &p, int button )
{
	for ( auto r: _reactors )
	{
		if ( r->mousePress( p, button ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

bool container::mouseRelease( const draw::point &p, int button )
{
	for ( auto r: _reactors )
	{
		if ( r->mouseRelease( p, button ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

bool container::mouseMove( const draw::point &p )
{
	for ( auto r: _reactors )
	{
		if ( r->mouseMove( p ) )
			return true;
	}
	return false;
}

////////////////////////////////////////

}

