
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::dispatcher.
///
/// Dispatcher implemented using Xlib.
class dispatcher : public ::platform::dispatcher
{
public:
	/// @brief Constructor.
	dispatcher( const std::shared_ptr<Display> &dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	~dispatcher( void );

	int execute( void ) override;
	void exit( int code ) override;
	void add_waitable( const std::shared_ptr<waitable> &w ) override;
	void remove_waitable( const std::shared_ptr<waitable> &w ) override;

	/// @brief Add a window.
	///
	/// Add a window for the dispatcher to handle events.
	void add_window( const std::shared_ptr<window> &w );

private:
	bool drainEvents( void );

	int _exit_code = 0;
	int _wait_pipe[2];
	std::shared_ptr<Display> _display;
	Atom _atom_delete_window;
	XIM _xim;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::map<Window, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

} }

