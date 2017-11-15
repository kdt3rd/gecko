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
#include <type_traits>
#include <functional>

// these are only partly needed by this header, but let's make it easier for
// people so they only have to include one header for now...
#include "cursor.h"
#include "screen.h"
#include "timer.h"
#include "window.h"
#include "dispatcher.h"

namespace platform
{

class keyboard;
class mouse;
class tray;
class menu;

////////////////////////////////////////

/// @brief Abstract class representing a system.
///
/// A system can enumerate screens, create windows, and dispatch events from user interations and timers.
class system
{
public:
	using opengl_func_ptr = void (*)( void );
	using opengl_query = opengl_func_ptr (*)( const char * );

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

	virtual opengl_query gl_proc_address( void ) = 0;

	/// @brief Return screens available to the system.
	///
	/// Returns a vector of screens available to the system.
	/// @return Vector of screens
	virtual std::vector<std::shared_ptr<screen>> screens( void ) = 0;

	/// @brief Creates a new cursor.
	///
	/// This can be assigned to be active when over a window (instead
	/// of the default) or be pushed and popped as a global thing
	/// (i.e. during drag and drop or something).
	///
	virtual std::shared_ptr<cursor> new_cursor( void ) = 0;

	/// @brief retrieve a pre-built cursor
	///
	/// this uses the system theme as appropriate
	/// TODO: add ability to query and switch themes
	virtual std::shared_ptr<cursor> builtin_cursor( standard_cursor sc ) = 0;

	/// @group selection interface

	/// @brief simple string selection handling
	///
	/// This sets a string as the available selection to the system
	virtual void set_selection( const std::string &data ) = 0;
	/// @brief mime-type based selection handling
	///
	/// This allows for rich selection transfer between applications
	/// it is expected that the function will remain valid until one
	/// of a few conditions happens:
	///  - @sa clear_selection is called
	///  - another selection is set
	///  - the system exits
	virtual void set_selection( const std::vector<uint8_t> &data,
								const std::vector<std::string> &avail_mime_types,
								const std::function<std::vector<uint8_t> (const std::vector<uint8_t> &, const std::string &)> &convert ) = 0;

	/// @brief clear selection
	virtual void clear_selection( void ) = 0;
	/// @brief query selection types available for pasting
	///
	/// on platforms where it makes sense, mouseSel is the mouse selection,
	/// compared to just querying the normal clipboard
	///
	/// if the reqTypes is empty, a generic string is requested
	/// if one of the reqTypes is not available, then an empty result is returned
	/// otherwise, the first type encountered is returned
	virtual std::pair<std::vector<uint8_t>, std::string> query_selection( bool mouseSel, const std::vector<std::string> &reqTypes = std::vector<std::string>() ) = 0;
	/// @brief same as other @sa query_selection, but with a custom clipboard name
	virtual std::pair<std::vector<uint8_t>, std::string> query_selection( const std::string &clipboardName, const std::vector<std::string> &reqTypes = std::vector<std::string>() ) = 0;

	/// @endgroup

	/// @group Drag and Drop interface

	/// @brief begin a drag motion.
	///
	/// Event processing continues as normal, but this routine will
	/// not return until the user releases the mouse button.
	virtual void begin_drag( const std::vector<uint8_t> &data,
							 const std::vector<std::string> &avail_mime_types,
							 const std::function<std::vector<uint8_t> (const std::vector<uint8_t> &, const std::string &)> &convert,
							 const std::shared_ptr<cursor> &cursor = std::shared_ptr<cursor>() ) = 0;

	/// @brief query available mime types in response to a drop request event
	virtual std::vector<std::string> query_available_drop_types( void ) = 0;
	/// @brief accept a drop of a particular type in response to a drop request event
	virtual std::vector<uint8_t> accept_drop( const std::string &type ) = 0;

	/// @endgroup

	/// @brief Creates a new system-level menu item.
	///
	/// Some systems allow a system level menu. for example, on OS/X,
	/// this will be the main menu across the top of the screen. Under
	/// some linux configurations (i.e. ubuntu unity), this is
	/// similar. Windows does not have this concept, and so would
	/// return null
	///
	/// @return a pointer to they system menu item. Implementations
	/// are allowed to return null
	virtual std::shared_ptr<menu> new_system_menu( void ) = 0;

	/// @brief Return a reference to a system tray, if it exists on
	/// the system.
	///
	/// Many systems allow an application to register a system tray
	/// icon, which can have menus and other behaviors.
	///
	/// @return new tray object. This may be null if the system tray
	/// is not available for the current system
	virtual std::shared_ptr<tray> new_system_tray_item( void ) = 0;

	/// @brief Create a new window.
	///
	/// Creates a new window.  Newly created windows are hidden and of size 0x0.
	/// @return A new window
	virtual std::shared_ptr<window> new_window( void ) = 0;

	/// @brief Forcibly destroy a window.
	///
	/// This is meant to be used when closing a window manually,
	/// either due to a user hotkey or anything that isn't the user
	/// closing the window using the window manager
	virtual void destroy_window( const std::shared_ptr<window> &w ) = 0;

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

	/// @brief queries the current keyboard modifiers
	///
	/// Returns a mask that is the logical or of the modifiers defined
	/// in scancode.h
	virtual uint8_t modifier_state( void ) = 0;

	/// @brief queries the current mouse position
	///
	/// This should only generally be necessary outside of the event
	/// system.
	virtual bool query_mouse( uint8_t &buttonMask, uint8_t &modifiers, int &x, int &y, int &screen ) = 0;

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
