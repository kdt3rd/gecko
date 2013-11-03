
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace xcb
{

////////////////////////////////////////

/// @brief XCB dispatcher.
///
/// Dispatcher implemented using XCB.
class dispatcher : public platform::dispatcher
{
public:
	dispatcher( xcb_connection_t *c, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	virtual ~dispatcher( void );

	int execute( void );
	void exit( int code );

	void add_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
	xcb_connection_t *_connection = nullptr;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::map<xcb_window_t, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

}
