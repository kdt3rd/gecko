//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/system.h>
#include <wayland-client.h>
#include <EGL/egl.h>
#include "dispatcher.h"

namespace platform { namespace wayland
{

////////////////////////////////////////

/// @brief Wayland implementation of platform::system.
class system : public ::platform::system
{
public:
	system( const std::string &d );
	~system( void );

	bool is_working( void ) const
	{
		return static_cast<bool>( _display );
	}

	opengl_query gl_proc_address( void ) override;

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override
	{
		return _screens;
	}

	std::shared_ptr<::platform::cursor> new_cursor( void ) override;
	std::shared_ptr<::platform::cursor> builtin_cursor( standard_cursor sc ) override;

	void set_selection( const std::string &data ) override;
	void set_selection( const std::vector<uint8_t> &data,
						const std::vector<std::string> &avail_mime_types,
						const std::function<std::vector<uint8_t> (const std::vector<uint8_t> &, const std::string &)> &convert ) override;

	void clear_selection( void ) override;

	std::pair<std::vector<uint8_t>, std::string> query_selection( bool mouseSel, const std::vector<std::string> &reqTypes = std::vector<std::string>() ) override;
	std::pair<std::vector<uint8_t>, std::string> query_selection( const std::string &clipboardName, const std::vector<std::string> &reqTypes = std::vector<std::string>() ) override;

	void begin_drag( const std::vector<uint8_t> &data,
					 const std::vector<std::string> &avail_mime_types,
					 const std::function<std::vector<uint8_t> (const std::vector<uint8_t> &, const std::string &)> &convert,
					 const std::shared_ptr<::platform::cursor> &cursor = std::shared_ptr<::platform::cursor>() ) override;

	std::vector<std::string> query_available_drop_types( void ) override;
	std::vector<uint8_t> accept_drop( const std::string &type ) override;

	std::shared_ptr<::platform::menu> new_system_menu( void ) override;
	std::shared_ptr<::platform::tray> new_system_tray_item( void ) override;

	std::shared_ptr<::platform::window> new_window( void ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<::platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<::platform::mouse> get_mouse( void ) override;

	uint8_t modifier_state( void ) override;
	bool query_mouse( uint8_t &buttonMask, uint8_t &modifiers, int &x, int &y, int &screen ) override;

	void set_compositor( struct wl_compositor *c ) { _compositor = c; }
	void set_shell( struct wl_shell *s ) { _shell = s; }

	static bool is_available( void );

private:
	std::shared_ptr<struct wl_display> _display;
	struct wl_compositor *_compositor = nullptr;
	struct wl_shell *_shell = nullptr;
	EGLDisplay _egl_disp = nullptr;
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

