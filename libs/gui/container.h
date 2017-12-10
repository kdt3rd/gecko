//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include <gl/api.h>
#include <layout/grid.h>
#include <layout/box.h>
#include <layout/tree.h>
#include <map>

namespace gui
{

////////////////////////////////////////

class base_container : public widget
{
public:
	base_container( void );
	explicit base_container( std::unique_ptr<layout::area> &&a );
	~base_container( void ) override;

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;
	bool mouse_press( const point &p, int button ) override;
	bool mouse_release( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;
	bool mouse_wheel( int amount ) override;
	bool key_press( platform::scancode c ) override;
	bool key_release( platform::scancode c ) override;
	bool text_input( char32_t c ) override;
	bool update_layout( double duration ) override;

	void remove( size_t w );

protected:
	std::vector<std::shared_ptr<widget>> _widgets;
	std::shared_ptr<widget> _mouse_grab;
	std::shared_ptr<widget> _key_focus;
};

////////////////////////////////////////

template<typename TheLayout>
class container : public base_container
{
public:
	template<typename ...Args>
	container( Args ...args )
		: base_container( std::unique_ptr<TheLayout>( new TheLayout( std::forward<Args>( args )... ) ) )
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

	template<typename ...Args>
	size_t add( const std::shared_ptr<widget> &w, Args ...args )
	{
		_widgets.push_back( w );
		_layout->add( w->layout_target(), std::forward<Args>( args )... );
		return _widgets.size() - 1;
	}

	void set_padding( double l, double r, double t, double b )
	{
		_layout->set_padding( l, r, t, b );
	}

	void set_spacing( double h, double v )
	{
		_layout->set_spacing( h, v );
	}

protected:
	std::shared_ptr<TheLayout> _layout;
};

////////////////////////////////////////

extern template class container<layout::grid>;
extern template class container<layout::box>;
extern template class container<layout::tree>;

using grid = container<layout::grid>;
using box = container<layout::box>;
using tree = container<layout::tree>;

////////////////////////////////////////

}

