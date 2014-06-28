
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of dispatcher.
class dispatcher : public ::platform::dispatcher
{
public:
	dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	~dispatcher( void );

	int execute( void );
	void exit( int code );

	void add_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
//	bool _continue_running = true;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::map<void*,std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

} }

