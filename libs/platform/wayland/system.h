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

	std::shared_ptr<::platform::window> new_window( void ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<::platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<::platform::mouse> get_mouse( void ) override;

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

