//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "context.h"
#include <base/scope_guard.h>
#include <platform/keyboard.h>
#include <memory>

namespace platform
{
	class window;
}

namespace gui
{

class widget;

////////////////////////////////////////

class window : public context
{
public:
	window( const std::shared_ptr<platform::window> &w );
	virtual ~window( void );

	void set_title( const std::string &t );

	void show( void );
	void hide( void );

	void move( double x, double y );
	void resize( double w, double h );

	void set_widget( const std::shared_ptr<widget> &w );
	std::shared_ptr<widget> get_widget( void ) { return _widget; }

	double width( void ) const;
	double height( void ) const;

	void invalidate( const base::rect &r ) override;

	typedef base::scope_guard<std::function<void(void)>> bound_context;
	bound_context bind( void );

protected:
	void paint( void );
	void resized( double w, double h );

	void mouse_press( const base::point &p, int button );
	void mouse_release( const base::point &p, int button );
	void mouse_moved( const base::point &p );
	void mouse_wheel( int amount );

	void key_pressed( platform::scancode c );
	void key_released( platform::scancode c );
	void text_entered( char32_t c );

	std::shared_ptr<platform::window> _window;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

