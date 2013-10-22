
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "screen.h"
#include "window.h"
#include "timer.h"
#include "dispatcher.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform
{

////////////////////////////////////////

/// @brief Abstract class representing a system.
/// A system can enumerate screens, create windows, and dispatch events from user interations and timers.
class system
{
public:
	/// @brief Constructor
	/// Construct a base 
	/// @param name Name of the system
	/// @param desc Description of the system
	system( std::string name, std::string desc );
	
	/// @brief Destructor
	virtual ~system( void );

	/// @brief Name of the system
	/// The short name of the system.
	/// @return Name of the system
	const std::string &name( void ) const { return _name; }

	/// @brief Description of the system
	/// A brief description of the system.
	/// @return Description of the system
	const std::string &description( void ) const { return _desc; }

	/// @brief Return screens available to the system
	/// Returns a vector of screens available to the system.
	/// @return Vector of screens
	virtual std::vector<std::shared_ptr<screen>> screens( void ) = 0;

	
	/// @brief Create a new window
	/// Creates a new window.  Newly created windows are hidden and of size 0x0.
	/// @return A new window
	virtual std::shared_ptr<window> new_window( void ) = 0;

	/// @brief Create a new timer
	/// Create a new timer.  Newly created timers are stopped and must be started.
	/// @return A new timer
	virtual std::shared_ptr<timer> new_timer( void ) = 0;

	/// @brief Get the dispatcher
	/// Get the dispatcher for the system.
	/// @return The dispatcher
	virtual std::shared_ptr<dispatcher> get_dispatcher( void ) = 0;

	/// @brief Get the keyboard
	/// Get the main system keyboard.
	/// @return The keyboard
	virtual std::shared_ptr<keyboard> get_keyboard( void ) = 0;

	/// @brief Get the mouse
	/// Get the main system mouse.
	/// @return The mouse
	virtual std::shared_ptr<mouse> get_mouse( void ) = 0;

private:
	std::string _name;
	std::string _desc;
};

////////////////////////////////////////

}
