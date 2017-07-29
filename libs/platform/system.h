//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "screen.h"
#include "window.h"
#include "dispatcher.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"

namespace platform
{

////////////////////////////////////////

/// @brief Abstract class representing a system.
///
/// A system can enumerate screens, create windows, and dispatch events from user interations and timers.
class system
{
public:
	/// @brief Constructor.
	///
	/// Construct a system with the given name/description.
	/// @param name Name of the system
	/// @param desc Description of the system
	system( std::string name, std::string desc );

	/// @brief Destructor.
	virtual ~system( void );

	/// @brief Is this system functional.
	virtual bool is_working( void ) const = 0;

	/// @brief Name of the system.
	///
	/// The short name of the system.
	/// @return Name of the system
	const std::string &name( void ) const { return _name; }

	/// @brief Description of the system.
	///
	/// A description of the system.
	/// @return Description of the system
	const std::string &description( void ) const { return _desc; }

	/// @brief Return screens available to the system.
	///
	/// Returns a vector of screens available to the system.
	/// @return Vector of screens
	virtual std::vector<std::shared_ptr<screen>> screens( void ) = 0;

	/// @brief Create a new window.
	///
	/// Creates a new window.  Newly created windows are hidden and of size 0x0.
	/// @return A new window
	virtual std::shared_ptr<window> new_window( void ) = 0;

	/// @brief Get the dispatcher.
	///
	/// Get the dispatcher for the system.
	/// @return The dispatcher
	virtual std::shared_ptr<dispatcher> get_dispatcher( void ) = 0;

	/// @brief Get the keyboard.
	///
	/// Get the main system keyboard.
	/// @return The keyboard
	virtual std::shared_ptr<keyboard> get_keyboard( void ) = 0;

	/// @brief Get the mouse.
	///
	/// Get the main system mouse.
	/// @return The mouse
	virtual std::shared_ptr<mouse> get_mouse( void ) = 0;

	/// @brief creates a generic timer.
	///
	/// NB: The owner of the shared pointer is responsible for
	/// registering and unregistering as a waitable with the
	/// dispatcher
	std::shared_ptr<timer> create_timer( void );

	/// @brief creates a one off timer.
	///
	/// This will register a timer and call the callback when it
	/// expires, managing the timer for the user (not reusable)
	void create_one_off_timer( double in_seconds, const std::function<void(void)> &e );
	void create_one_off_timer( timer::duration in_seconds, const std::function<void(void)> &e );
	void create_one_off_timer( timer::time_point when, const std::function<void(void)> &e );

private:
	void dispatch_oneoff_timer( const std::shared_ptr<timer> &t, const std::function<void(void)> &e );
	std::string _name;
	std::string _desc;
};

////////////////////////////////////////

}
