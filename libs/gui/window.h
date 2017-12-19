//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "context.h"
#include "widget_ptr.h"
#include <base/scope_guard.h>
#include <platform/keyboard.h>
#include <platform/cursor.h>
#include <gl/api.h>
#include <memory>

namespace platform
{
	class window;
}

namespace gui
{

////////////////////////////////////////

class window : public context
{
public:
	window( const std::shared_ptr<platform::window> &w );
	virtual ~window( void );

	void set_title( const std::string &t );

	void set_default_cursor( const std::shared_ptr<platform::cursor> &c );
	void push_cursor( const std::shared_ptr<platform::cursor> &c );
	void pop_cursor( void );

	void show( void );
	void hide( void );

	void move( coord_type x, coord_type y );
	void resize( coord_type w, coord_type h );

	void set_widget( const std::shared_ptr<widget> &w );
	template <typename Y>
	inline void set_widget( const widget_ptr<Y> &w ) { set_widget( static_cast<std::shared_ptr<Y>>( w ) ); }
	std::shared_ptr<widget> get_widget( void ) { return _widget; }

	coord_type width( void ) const;
	coord_type height( void ) const;

	void invalidate( const rect &r ) override;

	typedef base::scope_guard<std::function<void(void)>> bound_context;
	bound_context bind( void );

protected:
	void paint( void );
	void resized( coord_type w, coord_type h );

	void mouse_press( const point &p, int button );
	void mouse_release( const point &p, int button );
	void mouse_moved( const point &p );
	void mouse_wheel( int amount );

	void key_pressed( platform::scancode c );
	void key_released( platform::scancode c );
	void text_entered( char32_t c );

	gl::api _ogl;
	std::shared_ptr<platform::window> _window;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

