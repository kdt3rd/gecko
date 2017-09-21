//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/system.h>
#include <X11/Xlib.h>
#include "dispatcher.h"

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::system.
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

	std::shared_ptr<::platform::menu> new_system_menu( void ) override;
	std::shared_ptr<::platform::tray> new_system_tray_item( void ) override;

	std::shared_ptr<::platform::window> new_window( void ) override;

	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<::platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<::platform::mouse> get_mouse( void ) override;

private:
	std::shared_ptr<Display> _display;
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

