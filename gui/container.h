
#pragma once

#include "widget.h"
#include <reaction/passive.h>
#include <map>

namespace gui
{

////////////////////////////////////////

template<typename layout>
class container : public widget, public layout
{
public:
	using layout::layout;

	virtual ~container( void )
	{
	}

	virtual void layout( void )
	{
		for ( auto w: _widgets )
			w->layout();

		this->recompute_minimum( *this );
		this->recompute_layout( *this );
	}

	virtual bool mouse_press( const draw::point &p, int button )
	{
		for ( auto w: _widgets )
		{
			if ( w->mouse_press( p, button ) )
				return true;
		}
		return widget::mouse_press( p, button );
	}

	virtual bool mouse_release( const draw::point &p, int button )
	{
		for ( auto w: _widgets )
		{
			if ( w->mouse_release( p, button ) )
				return true;
		}
		return widget::mouse_release( p, button );
	}

	virtual bool mouse_move( const draw::point &p )
	{
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
	}

private:
	std::vector<std::shared_ptr<widget>> _widgets;
};

////////////////////////////////////////

}

