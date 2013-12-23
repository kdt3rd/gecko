
#pragma once

#include "widget.h"
#include "layouts.h"
#include <reaction/passive.h>
#include <map>

namespace gui
{

////////////////////////////////////////

template<typename the_layout>
class container : public widget, public the_layout
{
public:
	using the_layout::the_layout;

	virtual ~container( void )
	{
	}

	virtual void set_delegate( delegate *d )
	{
		for ( auto w: _widgets )
			w->set_delegate( d );
	}

	virtual void compute_minimum( void )
	{
		for ( auto w: _widgets )
			w->compute_minimum();
		this->recompute_minimum( *this );
	}

	virtual void compute_layout( void )
	{
		this->recompute_layout( *this );
		for ( auto w: _widgets )
			w->compute_layout();
	}

	virtual bool mouse_press( const draw::point &p, int button )
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

	virtual bool mouse_release( const draw::point &p, int button )
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

	virtual bool mouse_move( const draw::point &p )
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

	virtual void paint( const std::shared_ptr<draw::canvas> &c )
	{
		for ( auto w: _widgets )
			w->paint( c );
	}

protected:
	void added( const std::shared_ptr<widget> &w )
	{
		_widgets.push_back( w );
		w->set_delegate( _delegate );
	}

private:
	std::vector<std::shared_ptr<widget>> _widgets;
	std::shared_ptr<widget> _mouse_grab;
};

////////////////////////////////////////

typedef container<form_layout> form;
typedef container<grid_layout> grid;
typedef container<box_layout> simple_container;

extern template class container<form_layout>;
extern template class container<grid_layout>;
extern template class container<box_layout>;

////////////////////////////////////////

}

