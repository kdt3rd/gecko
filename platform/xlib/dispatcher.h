
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::dispatcher.
///
/// Dispatcher implemented using Xlib.
class dispatcher : public platform::dispatcher
{
public:
	/// @brief Constructor.
	dispatcher( Display *dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	virtual ~dispatcher( void );

	int execute( void );
	void exit( int code );

	/// @brief Add a window.
	///
	/// Add a window for the dispatcher to handle events.
	void add_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
	Display *_display = nullptr;
	Atom _atom_delete_window;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::map<Window, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

}
