//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include <gl/api.h>
#include <layout/grid_layout.h>
#include <layout/box_layout.h>
#include <layout/tree_layout.h>
#include <map>

namespace gui
{

////////////////////////////////////////

template<typename TheLayout>
class container : public widget
{
public:
	template<typename ...Args>
	container( Args ...args )
		: widget( std::unique_ptr<TheLayout>( new TheLayout( std::forward<Args>( args )... ) ) )
	{
		_layout = std::dynamic_pointer_cast<TheLayout>( layout_target() );
	}

	~container( void )
	{
	}

	TheLayout &layout( void )
	{
		return *_layout;
	}

	void build( gl::api &ogl ) override
	{
		for ( auto w: _widgets )
			w->build( ogl );
	}

	void paint( gl::api &ogl ) override
	{
		ogl.push_scissor( x(), y(), width(), height() );
		ogl.clear_color( context::current().get_style().background_color() );
		ogl.clear();
		for ( auto w: _widgets )
			w->paint( ogl );
		ogl.pop_scissor();
	}

	bool mouse_press( const base::point &p, int button ) override
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

	bool mouse_release( const base::point &p, int button ) override
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

	bool mouse_move( const base::point &p ) override
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

	bool mouse_wheel( int amount ) override
	{
		for ( auto w: _widgets )
		{
			if ( w->mouse_wheel( amount ) )
				return true;
		}
		return widget::mouse_wheel( amount );
	}

	bool key_press( platform::scancode c ) override
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

	bool key_release( platform::scancode c ) override
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

	bool text_input( char32_t c ) override
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

	template<typename ...Args>
	size_t add( const std::shared_ptr<widget> &w, Args ...args )
	{
		_widgets.push_back( w );
		_layout->add( w->layout_target(), std::forward<Args>( args )... );
		return _widgets.size() - 1;
	}

	void remove( size_t w )
	{
		_widgets.erase( _widgets.begin() + w );
	}

	void set_padding( double l, double r, double t, double b )
	{
		_layout->set_padding( l, r, t, b );
	}

	void set_spacing( double h, double v )
	{
		_layout->set_spacing( h, v );
	}

	bool update_layout( double duration ) override
	{
		bool result = widget::update_layout( duration );
		for ( auto &w: _widgets )
			result = w->update_layout( duration ) | result;
		return result;
	}

protected:
	std::shared_ptr<TheLayout> _layout;
	std::vector<std::shared_ptr<widget>> _widgets;
	std::shared_ptr<widget> _mouse_grab;
	std::shared_ptr<widget> _key_focus;
};

////////////////////////////////////////

extern template class container<layout::grid>;
extern template class container<layout::box>;
extern template class container<layout::tree>;

typedef container<layout::grid> grid;
typedef container<layout::box> box;
typedef container<layout::tree> tree;

////////////////////////////////////////

}

