// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "types.h"
#include "style.h"
#include <platform/context.h>
#include <base/scope_guard.h>

namespace gui
{

class event;
class widget;

////////////////////////////////////////

/// @brief Context for gui.
class context
{
public:
	context( void ) = default;
	virtual ~context( void );
	context( const context & ) = delete;
	context( context && ) = delete;
	context &operator=( const context & ) = delete;
	context &operator=( context && ) = delete;

	virtual void invalidate( const rect &r ) = 0;

	virtual coord from_native_horiz( platform::coord_type c ) const = 0;
	virtual coord from_native_vert( platform::coord_type c ) const = 0;
	virtual point from_native( const platform::point &p ) const = 0;
	virtual point from_native( platform::coord_type x, platform::coord_type y ) const = 0;
	virtual size from_native( const platform::size &s ) const = 0;
	virtual rect from_native( const platform::rect &r ) const = 0;

	virtual platform::coord_type to_native_horiz( const coord &c ) const = 0;
	virtual platform::coord_type to_native_vert( const coord &c ) const = 0;
	virtual platform::point to_native( const point &p ) const = 0;
	virtual platform::size to_native( const size &s ) const = 0;
	virtual platform::rect to_native( const rect &r ) const = 0;

	platform::context::clip_region_guard push_clip( const rect &r );
	platform::context::clip_region_guard push_clip( coord x, coord y, coord w, coord h )
	{ return push_clip( rect( x, y, w, h ) ); }

	virtual platform::context &hw_context( void ) = 0;

	virtual void grab_source( const event &e, std::shared_ptr<widget> w ) = 0;
	virtual void release_source( const event &e ) = 0;
	virtual void set_focus( std::shared_ptr<widget> w ) = 0;

	const style &get_style( void ) const
	{
		return _style;
	}

	style &get_style( void )
	{
		return _style;
	}

	template<typename func>
	void in_context( func &&f )
	{
		push_context();
		on_scope_exit { pop_context(); };

		auto guard = hw_context().begin_render();

		std::forward<func>(f)();
	}

	static context &current( void );

protected:
	void push_context( void );
	void pop_context( void );

	style _style;
};

////////////////////////////////////////

}
