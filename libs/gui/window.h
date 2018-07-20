//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "context.h"
#include "widget_ptr.h"
#include <base/scope_guard.h>
#include <platform/keyboard.h>
#include <platform/context.h>
#include <platform/cursor.h>
#include <platform/scancode.h>
#include <gl/api.h>
#include <memory>
#include <map>
#include <functional>

namespace platform
{
	class window;
}

namespace gui
{

class event;

////////////////////////////////////////

class window : public context
{
public:
	using hotkey_handler = std::function<void(const point &)>;

	window( const std::shared_ptr<platform::window> &w );
	~window( void ) override;

	void set_title( const std::string &t );

	void set_default_cursor( const std::shared_ptr<platform::cursor> &c );
	void push_cursor( const std::shared_ptr<platform::cursor> &c );
	void pop_cursor( void );

	void show( void );
	void hide( void );

	void move( coord x, coord y );
	void resize( coord w, coord h );

	// enables a hot key that triggers in this window, enabling
	// different handlers for different windows.
	// TODO: allow multiple scancode combinations? (up to 6, limit of usb)
	void set_local_hotkey( platform::scancode sc, hotkey_handler f );

	void set_widget( const std::shared_ptr<widget> &w );
	template <typename Y>
	inline void set_widget( const widget_ptr<Y> &w ) { set_widget( static_cast<std::shared_ptr<Y>>( w ) ); }
	std::shared_ptr<widget> get_widget( void ) { return _widget; }

	coord width( void ) const;
	coord height( void ) const;

	void invalidate( const rect &r ) override;

	coord from_native_horiz( platform::coord_type c ) const override;
	coord from_native_vert( platform::coord_type c ) const override;
	point from_native( const platform::point &p ) const override;
	point from_native( platform::coord_type x, platform::coord_type y ) const override;
	size from_native( const platform::size &s ) const override;
	rect from_native( const platform::rect &r ) const override;

	platform::coord_type to_native_horiz( const coord &c ) const override;
	platform::coord_type to_native_vert( const coord &c ) const override;
	platform::point to_native( const point &p ) const override;
	platform::size to_native( const size &s ) const override;
	platform::rect to_native( const rect &r ) const override;

	platform::context &hw_context( void ) override;

	void grab_source( const event &e, std::shared_ptr<widget> w ) override;
	void release_source( const event &e ) override;
	void set_focus( std::shared_ptr<widget> w ) override;

	platform::context::render_guard bind( void );

protected:
	void monitor_changed( void );

	bool process_event( const event &e );

	/// if you subclass window and make a "main window", or otherwise
	/// count the number of windows and then prompt the user to save,
	/// you should override this and return false if the window close
	/// request should be ignored / cancelled.
	///
	/// by default, returns true, indicating the window can close.
	virtual bool close_request( const event &e );

	void paint( const rect &r );
	void resized( const size &s );

	void key_down( const event &e );
	void key_repeat( const event &e );

	std::shared_ptr<platform::window> _window;
	std::shared_ptr<widget> _widget;

	std::shared_ptr<widget> _mouse_grab;
	std::shared_ptr<widget> _key_focus;

	bool _internal_paint = false;

	std::map<platform::scancode, hotkey_handler> _hotkeys;
};

////////////////////////////////////////

}
