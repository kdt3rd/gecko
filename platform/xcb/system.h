
#pragma once

#include <platform/system.h>
#include <xcb/xcb.h>
#include "dispatcher.h"
#include "font_manager.h"

namespace xcb
{

////////////////////////////////////////

/// @brief XCB implementation of platform::system.
class system : public platform::system
{
public:
	system( void );
	virtual ~system( void );

	virtual std::vector<std::shared_ptr<platform::screen>> screens( void ) { return _screens; }
	virtual std::shared_ptr<platform::window> new_window( void );
	virtual std::shared_ptr<platform::timer> new_timer( void );

	virtual std::shared_ptr<platform::dispatcher> get_dispatcher( void );
	virtual std::shared_ptr<platform::keyboard> get_keyboard( void );
	virtual std::shared_ptr<platform::mouse> get_mouse( void );
	virtual std::shared_ptr<platform::font_manager> get_font_manager( void );

private:
	xcb_connection_t *_connection;
	xcb_screen_t *_screen;
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::shared_ptr<font_manager> _font_manager;
	std::vector<std::shared_ptr<platform::screen>> _screens;
};

////////////////////////////////////////

}

////////////////////////////////////////

