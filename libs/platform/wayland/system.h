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

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override
	{
		return _screens;
	}
	const std::shared_ptr<::platform::screen> &default_screen( void ) override;

	std::shared_ptr<::platform::cursor> new_cursor( void ) override;
	std::shared_ptr<::platform::cursor> builtin_cursor( standard_cursor sc ) override;

	void set_selection( selection sel ) override;

	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const std::vector<std::string> &allowedMimeTypes = std::vector<std::string>(),
																  const std::string &clipboardName = std::string() ) override;
	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const selection_type_function &chooseMimeType,
																  const std::string &clipboardName = std::string() ) override;

	const std::vector<std::string> &default_string_types( void ) override;
	selection_type_function default_string_selector( void ) override;
	mime_converter default_string_converter( void ) override;

	void begin_drag( selection sel,
					 const std::shared_ptr<::platform::cursor> &cursor = std::shared_ptr<::platform::cursor>() ) override;
	std::pair<std::vector<uint8_t>, std::string> query_drop( const selection_type_function &chooseMimeType = selection_type_function() ) override;

	std::shared_ptr<::platform::menu> new_system_menu( void ) override;
	std::shared_ptr<::platform::tray> new_system_tray_item( void ) override;

	std::shared_ptr<::platform::window> new_window( window_type wintype = window_type::normal, const std::shared_ptr<::platform::screen> &s = std::shared_ptr<::platform::screen>() ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;

	void set_compositor( struct wl_compositor *c ) { _compositor = c; }
	void set_shell( struct wl_shell *s ) { _shell = s; }

	static bool is_available( void );

private:
	std::shared_ptr<struct wl_display> _display;
	struct wl_compositor *_compositor = nullptr;
	struct wl_shell *_shell = nullptr;
	EGLDisplay _egl_disp = nullptr;
	std::shared_ptr<dispatcher> _dispatcher;
	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

