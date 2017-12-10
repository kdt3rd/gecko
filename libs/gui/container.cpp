//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "container.h"

namespace gui
{

////////////////////////////////////////

base_container::base_container( void )
{
}

////////////////////////////////////////

base_container::base_container( std::unique_ptr<layout::area> &&a )
	: widget( std::move( a ) )
{
}

////////////////////////////////////////

base_container::~base_container( void )
{
}

////////////////////////////////////////

void base_container::build( gl::api &ogl )
{
	for ( auto w: _widgets )
		w->build( ogl );
}

////////////////////////////////////////

void base_container::paint( gl::api &ogl )
{
	ogl.push_scissor( x(), y(), width(), height() );
	ogl.clear_color( context::current().get_style().background_color() );
	ogl.clear();
	for ( auto w: _widgets )
		w->paint( ogl );
	ogl.pop_scissor();
}

////////////////////////////////////////

bool base_container::mouse_press( const point &p, int button )
{
	for ( auto w: _widgets )
	{
		if ( w->mouse_press( p, button ) )
		{
			_mouse_grab = w;
			return true;
		}
	}
	return widget::mouse_press( p, button );
}

////////////////////////////////////////

bool base_container::mouse_release( const point &p, int button )
{
	if ( _mouse_grab )
	{
		auto tmp = _mouse_grab;
		_mouse_grab.reset();
		return tmp->mouse_release( p, button );
	}

	for ( auto w: _widgets )
	{
		if ( w->mouse_release( p, button ) )
			return true;
	}
	return widget::mouse_release( p, button );
}

////////////////////////////////////////

bool base_container::mouse_move( const point &p )
{
	if ( _mouse_grab )
		return _mouse_grab->mouse_move( p );

	for ( auto w: _widgets )
	{
		if ( w->mouse_move( p ) )
			return true;
	}
	return widget::mouse_move( p );
}

////////////////////////////////////////

bool base_container::mouse_wheel( int amount )
{
	for ( auto w: _widgets )
	{
		if ( w->mouse_wheel( amount ) )
			return true;
	}
	return widget::mouse_wheel( amount );
}

////////////////////////////////////////

bool base_container::key_press( platform::scancode c )
{
	if ( _key_focus )
		return _key_focus->key_press( c );

	for ( auto w: _widgets )
	{
		if ( w->key_press( c ) )
			return true;
	}
	return widget::key_press( c );
}

////////////////////////////////////////

bool base_container::key_release( platform::scancode c )
{
	if ( _key_focus )
		return _key_focus->key_release( c );

	for ( auto w: _widgets )
	{
		if ( w->key_release( c ) )
			return true;
	}
	return widget::key_release( c );
}

////////////////////////////////////////

bool base_container::text_input( char32_t c )
{
	if ( _key_focus )
		return _key_focus->text_input( c );

	for ( auto w: _widgets )
	{
		if ( w->text_input( c ) )
			return true;
	}
	return widget::text_input( c );
}

////////////////////////////////////////

bool base_container::update_layout( double duration )
{
	bool result = widget::update_layout( duration );
	for ( auto &w: _widgets )
		result = w->update_layout( duration ) | result;
	return result;
}

////////////////////////////////////////

void base_container::remove( size_t w )
{
	_widgets.erase( _widgets.begin() + w );
}

////////////////////////////////////////

template class container<layout::grid>;
template class container<layout::box>;
template class container<layout::tree>;

////////////////////////////////////////

}
