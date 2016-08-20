
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
	system( void );
	~system( void );

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override
	{
		return _screens;
	}

	std::shared_ptr<::platform::window> new_window( void ) override;

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

