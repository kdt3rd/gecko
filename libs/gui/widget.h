//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>
#include <layout/area.h>
#include <platform/keyboard.h>
#include "context.h"

namespace gl
{
class api;
};

namespace gui
{

////////////////////////////////////////

class widget : public layout::area
{
public:
	widget( void );
	virtual ~widget( void );

	virtual void build( gl::api &ogl );

	virtual void paint( gl::api &ogl );
	virtual bool mouse_press( const base::point &p, int button );
	virtual bool mouse_release( const base::point &p, int button );
	virtual bool mouse_move( const base::point &p );
	virtual bool mouse_wheel( int a );
	virtual bool key_press( platform::scancode c );
	virtual bool key_release( platform::scancode c );
	virtual bool text_input( char32_t c );

	void invalidate( const base::rect &r )
	{
		context::current().invalidate( r );
	}

	void invalidate( void )
	{
		context::current().invalidate( *this );
	}

protected:
	template<typename D>
	void callback_helper( const std::function<void(void)> &cb, D &d )
	{
		d.callback( cb );
	}

	template<typename D, typename ...Args>
	void callback_helper( const std::function<void(void)> &cb, D &d, Args &...args )
	{
		d.callback( cb );
		callback_helper( cb, args... );
	}

	template<typename ...Args>
	void callback_invalidate( Args &...args )
	{
		auto cb = [this]() { invalidate(); };
		callback_helper( cb, args... );
	}
};

////////////////////////////////////////

}

