
#pragma once

#include "widget.h"
#include "layouts.h"
#include <map>

namespace gui
{

////////////////////////////////////////

template<typename the_layout>
class container : public widget, public the_layout
{
public:
	using the_layout::the_layout;

	~container( void )
	{
	}

	void compute_minimum( void ) override
	{
		for ( auto w: _widgets )
			w->compute_minimum();
		this->recompute_minimum( *this );
	}

	void compute_layout( void ) override
	{
		this->recompute_layout( *this );
		for ( auto w: _widgets )
			w->compute_layout();
	}

	bool mouse_press( const core::point &p, int button ) override
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

	bool mouse_release( const core::point &p, int button ) override
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

	bool mouse_move( const core::point &p ) override
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

	void paint( const std::shared_ptr<draw::canvas> &c ) override
	{
		for ( auto w: _widgets )
			w->paint( c );
	}

protected:
	void added( const std::shared_ptr<widget> &w ) override
	{
		_widgets.push_back( w );
	}

	std::vector<std::shared_ptr<widget>> _widgets;
	std::shared_ptr<widget> _mouse_grab;
	std::shared_ptr<widget> _key_focus;
};

////////////////////////////////////////

typedef container<form_layout> form;
typedef container<grid_layout> grid;
typedef container<box_layout> simple_container;

extern template class container<form_layout>;
extern template class container<grid_layout>;
extern template class container<tree_layout>;
extern template class container<box_layout>;

////////////////////////////////////////

}

