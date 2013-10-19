
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"

namespace xcb
{

////////////////////////////////////////

class dispatcher : public platform::dispatcher
{
public:
	dispatcher( xcb_connection_t *c, const std::shared_ptr<keyboard> &k );
	virtual ~dispatcher( void );

	int execute( void );
	void exit( int code );

	void add_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
	xcb_connection_t *_connection = nullptr;
	std::shared_ptr<keyboard> _keyboard;
	std::map<xcb_window_t, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

}
