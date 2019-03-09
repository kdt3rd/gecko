// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <stack>

#include "types.h"
#include "scancode.h"
#include "event_target.h"
#include "context.h"

namespace platform
{

class cursor;
class screen;

enum class window_type
{
	normal,
	modal,
	popup,
	tooltip
};

////////////////////////////////////////

/// @brief A rectangular area of the screen.
class window : public event_target
{
public:
	/// @brief Constructor.
	explicit window( window_type wt, const std::shared_ptr<screen> &screen, const rect &p = rect( 0, 0, 512, 512 ) );

	/// @brief Destructor.
	~window( void ) override;

	window_type type( void ) const { return _win_type; }

	/// @brief default event handler for windows.
	///
	/// Dispatches and calls the various registered callback functions
	bool process_event( const event &e ) override;

	/// @brief returns the screen the window was created on.
	inline const std::shared_ptr<screen> &query_screen( void ) const { return _screen; }

	/// @brief creates a context (or returns a stashed version)
	///
	/// This is used to retrieve the rendering context appropriate for this
	/// system / window, including the api with which to use for rendering
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

	/// @brief Move the window.
	///
	/// Move the window to the given position.
	/// @param x New x position of the window
	/// @param y New y position of the window
	void move( coord_type x, coord_type y );
	void move( const point &p ) { move( p[0], p[1] ); }
	/// Moves to a screen distance from the upper left of the screen
	/// the window was created with
	void move( const phys_point &p );

	/// @brief Resize the window.
	///
	/// Resize the window to the given size.
	/// @param w New width of the window
	/// @param h New height of the window
	void resize( coord_type w, coord_type h );
	void resize( const size &s ) { resize( s.w(), s.h() ); }
	/// Resizes the window to the specified size based on the screen
	/// the window was created with
	void resize( const phys_size &s );

	/// @brief Set minimum window size.
	///
	/// The window will not be allowed to resize smaller than the minimum given.
	/// @param w Minimum width for the window
	/// @param h Minimum height for the window
	void set_minimum_size( coord_type w, coord_type h );
	void set_minimum_size( const phys_size &s );

	/// @brief Set the window title.
	///
	/// Set the window title shown in the title bar.
	/// @param t The window title
	virtual void set_title( const std::string &t ) = 0;

	void invalidate( const rect &r );
	void reset_expose( void ) { _invalid_rgn.clear(); }
	void add_expose( const rect &r ) { _invalid_rgn.include( r ); }
	const rect &get_expose( void ) { return _invalid_rgn; }

//	virtual void set_icon( const icon &i );

	/// @brief query the window x position
	coord_type x( void ) const { return _rect.x(); }
	/// @brief query the window y position
	coord_type y( void ) const { return _rect.y(); }
	/// @brief query the window width
	coord_type width( void ) const { return _rect.width(); }
	coord_type height( void ) const { return _rect.height(); }

	rect bounds( void ) const { return _rect; }
	phys_rect phys_bounds( void ) const;

	/// @brief Generic event handler
	///
	/// This is the most general event handler, in that all events
	/// come in here. Uses of a full-fledged gui widget system
	/// probably want to use this, and dispatch their own versions of
	/// the below, and see all the events.
	std::function<bool(const event &)> event_handoff;

protected:
	virtual void apply_minimum_size( coord_type w, coord_type h ) = 0;
	virtual void make_current( const std::shared_ptr<cursor> & ) = 0;

	virtual rect query_geometry( void ) = 0;

	/// may return false indicating the position didn't change
	/// should fill in @param r with the resulting geometry
	virtual bool update_geometry( rect &r ) = 0;

	/// @brief submit an async event into the system to (eventually)
	/// redraw the window
	///
	/// NB: @sa _accumulate_expose below
	virtual void submit_delayed_expose( const rect &r ) = 0;

	/// if we invalidate too much, we flood the system with events and
	/// the UI becomes laggy because it's processing events,
	/// subclasses should use this in submit_delayed_expose
	bool _accumulate_expose = false;
	rect _invalid_rgn;

private:
	window_type _win_type;
	rect _rect;

	std::stack< std::shared_ptr<cursor> > _cursors;
	std::shared_ptr<cursor> _default_cursor;

	std::shared_ptr<screen> _screen;
};

////////////////////////////////////////

}
