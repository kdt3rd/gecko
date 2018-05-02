//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <stack>

#include "types.h"
// TODO: should we just forward declare these, or is the places where
// this is included small enough we can use the convenience
#include "mouse.h"
#include "keyboard.h"
#include "scancode.h"
#include "event_target.h"
#include "context.h"

namespace platform
{

class cursor;
class screen;

////////////////////////////////////////

/// @brief A rectangular area of the screen.
class window : public event_target
{
public:
	/// @brief Constructor.
	explicit window( const std::shared_ptr<screen> &screen, const rect &p = rect( 0, 0, 512, 512 ) );

	/// @brief Destructor.
	virtual ~window( void );

	/// @brief default event handler for windows.
	///
	/// Dispatches and calls the various registered callback functions
	bool process_event( const event &e ) override;

	/// @brief returns the screen the window was created on.
	inline const std::shared_ptr<screen> &query_screen( void ) const { return _screen; }

	/// @brief creates a context (or returns a stashed version)
	///
	/// This is returned as a shared pointer, for caching
	/// (performance) purposes, however, it is not intended to have a
	/// lifetime longer than a window, and if used past the
	/// destruction of a window, errors should be expected.
	virtual context &hw_context( void ) = 0;

	/// @brief Set a cursor as the default for the window
	///
	/// This is the cursor that is displayed if no other cursor is pushed
	///
	void set_default_cursor( const std::shared_ptr<cursor> &c );
	/// @brief Set a new cursor for the window, storing them as a stack
	void push_cursor( const std::shared_ptr<cursor> &c );
	/// @brief restore the previous cursor
	void pop_cursor( void );

	// TODO: add support for menus...
//	std::shared_ptr<menu_definition> top_level_menu( void );
//	virtual bool include_menu_in_window_area( void ) const = 0;

	/// @brief Raise the window.
	///
	/// Raise the window above all other windows.
	virtual void raise( void ) = 0;

	/// @brief Lower the window.
	///
	/// Lower the window below all other windows.
	virtual void lower( void ) = 0;

	/// @brief Set window as popup.
	///
	/// Make the window without decorations.
	virtual void set_popup( void ) = 0;

	/// @brief Show the window.
	///
	/// Make the window visible.
	virtual void show( void ) = 0;

	/// @brief Hide the window.
	///
	/// Make the window invisible.
	virtual void hide( void ) = 0;

	/// @brief Query if the window is visible.
	///
	/// @return Whether the window is visible or not
	virtual bool is_visible( void ) = 0;

	/// @brief Make the window fullscreen.
	///
	/// Make the window fullscreen.
	virtual void fullscreen( bool fs ) = 0;

//	virtual rect geometry( void ) = 0;

//	virtual void set_geometry( const rect &r ) = 0;

	/// @brief Move the window.
	///
	/// Move the window to the given position.
	/// @param x New x position of the window
	/// @param y New y position of the window
	void move( coord_type x, coord_type y );
	void move( const point &p ) { move( p.x(), p.y() ); }

	/// @brief Resize the window.
	///
	/// Resize the window to the given size.
	/// @param w New width of the window
	/// @param h New height of the window
	void resize( coord_type w, coord_type h );
	void resize( const size &s ) { resize( s.w(), s.h() ); }

	/// @brief Set minimum window size.
	///
	/// The window will not be allowed to resize smaller than the minimum given.
	/// @param w Minimum width for the window
	/// @param h Minimum height for the window
	virtual void set_minimum_size( coord_type w, coord_type h ) = 0;

	/// @brief Set the window title.
	///
	/// Set the window title shown in the title bar.
	/// @param t The window title
	virtual void set_title( const std::string &t ) = 0;

	void invalidate( const rect &r );

//	virtual void set_icon( const icon &i );

	/// @brief query the window x position
	coord_type x( void ) const { return _rect.x(); }
	/// @brief query the window y position
	coord_type y( void ) const { return _rect.y(); }
	/// @brief query the window width
	coord_type width( void ) const { return _rect.width(); }
	coord_type height( void ) const { return _rect.height(); }

	/// @brief Generic event handler
	///
	/// This is the most general event handler, in that all events
	/// come in here. Uses of a full-fledged gui widget system
	/// probably want to use this, and dispatch their own versions of
	/// the below, and see all the events.
	std::function<bool(const event &)> event_handoff;

protected:
	virtual void make_current( const std::shared_ptr<cursor> & ) = 0;

	virtual void submit_delayed_expose( const rect &r ) = 0;

	virtual rect query_geometry( void ) = 0;
	/// may return false indicating the position didn't change
	virtual bool update_geometry( rect &r ) = 0;

private:
	rect _rect;

	std::stack< std::shared_ptr<cursor> > _cursors;
	std::shared_ptr<cursor> _default_cursor;

	std::shared_ptr<screen> _screen;
};

////////////////////////////////////////

}

