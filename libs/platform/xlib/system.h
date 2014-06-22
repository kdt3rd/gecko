
#pragma once

#include <platform/system.h>
#include <X11/Xlib.h>
#include "dispatcher.h"
#include "font_manager.h"

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::system.
class system : public platform::system
{
public:
	system( void );
	~system( void );

	std::vector<std::shared_ptr<platform::screen>> screens( void ) override
	{
		return _screens;
	}

	std::shared_ptr<platform::window> new_window( void ) override;
	std::shared_ptr<platform::timer> new_timer( void ) override;

	std::shared_ptr<platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<platform::mouse> get_mouse( void ) override;
	std::shared_ptr<platform::font_manager> get_font_manager( void ) override;

private:
	std::shared_ptr<Display> _display;
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::shared_ptr<font_manager> _font_manager;
	std::vector<std::shared_ptr<platform::screen>> _screens;
};

////////////////////////////////////////

}

////////////////////////////////////////

