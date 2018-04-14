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
#include "selection.h"

namespace platform
{

class keyboard;
class mouse;
class tray;
class menu;

////////////////////////////////////////

/// @brief Abstract class representing a GUI system backend.
///
/// A system can enumerate screens, create windows, and dispatch
/// events from user interations and timers.
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
	virtual const std::shared_ptr<screen> &default_screen( void ) = 0;

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

	/// @brief selection handling
	///
	/// This should provide a selection object to the rest of the system
	virtual void set_selection( selection sel ) = 0;

	/// @brief query system selection
	///
	/// when the selection type @param sel is custom, this indicates use of a
	/// custom clipboard name, at which point the clipboardName must
	/// be provided. otherwise, it may be left at default.
	///
	/// if the @param allowedMimeTypes list is empty, a utf-8 string
	/// will be requested.
	///
	/// @return the selection present in the indicated selection type
	/// or clipboard, along with the selected mime type.
	///
	virtual std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																		  const std::vector<std::string> &allowedMimeTypes = std::vector<std::string>(),
																		  const std::string &clipboardName = std::string() ) = 0;

	/// @brief query system selection
	///
	/// Similar to @sa query_selection, but instead of providing a
	/// list of allowed mime types, allows dynamic selection of mime
	/// type based on what is in the selection.
	///
	/// @return the selection present in the indicated selection type
	/// or clipboard, along with the selected mime type.
	///
	virtual std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																		  const selection_type_function &mimeSelector,
																		  const std::string &clipboardName = std::string() ) = 0;

	/// @brief provides a list of mime types and synonyms common to the platform
	virtual const std::vector<std::string> &default_string_types( void ) = 0;
	/// @brief provides a default function to select a utf8 string
	virtual selection_type_function default_string_selector( void ) = 0;

	using mime_converter = selection::mime_converter;
	/// @brief provides a default function to handle a utf8 string
	virtual mime_converter default_string_converter( void ) = 0;

	/// @endgroup

	/// @group Drag and Drop interface

	/// @brief begin a drag motion.
	///
	/// Event processing continues as normal, but this routine will
	/// not return until the user releases the mouse button.
	virtual void begin_drag( selection sel,
							 const std::shared_ptr<cursor> &c = std::shared_ptr<cursor>() ) = 0;

	/// @brief query available mime types in response to a drop request event
	virtual std::pair<std::vector<uint8_t>, std::string> query_drop( const selection_type_function &chooseMimeType = selection_type_function() ) = 0;

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
	///
	/// if no (null) screen is provided, uses the default screen.
	///
	/// @return A new window
	virtual std::shared_ptr<window> new_window( const std::shared_ptr<screen> &s = std::shared_ptr<screen>() ) = 0;

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
	virtual bool query_mouse( uint8_t &buttonMask, uint8_t &modifiers, coord_type &x, coord_type &y, int &screen ) = 0;

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
