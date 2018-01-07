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
#include "mouse.h"
#include "keyboard.h"
#include "scancode.h"
#include "event_target.h"

namespace platform
{

class cursor;

////////////////////////////////////////

/// @brief A rectangular area of the screen.
class window : public event_target
{
public:
	/// @brief Constructor.
	window( void );

	/// @brief Destructor.
	virtual ~window( void );

	bool process_event( const event &e ) override;

	/// @brief Set a cursor as the default for the window
	///
	/// This is the cursor that is displayed if no other cursor is pushed
	///
	void set_default_cursor( const std::shared_ptr<cursor> &c );
	/// @brief Set a new cursor for the window, storing them as a stack
	void push_cursor( const std::shared_ptr<cursor> &c );
	/// @brief restore the previous cursor
	void pop_cursor( void );

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
	virtual void move( coord_type x, coord_type y ) = 0;
	void move( const point &p ) { move( p.x(), p.y() ); }

	/// @brief Resize the window.
	///
	/// Resize the window to the given size.
	/// @param w New width of the window
	/// @param h New height of the window
	virtual void resize( coord_type w, coord_type h ) = 0;
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

	virtual void invalidate( const rect &r ) = 0;

//	virtual void set_icon( const icon &i );

	virtual void acquire( void ) = 0;
	virtual void release( void ) = 0;

	std::function<bool(const event &)> event_handoff;

	/// @brief Action for mouse press events.
	///
	/// Callback action for mouse button press events.
	std::function<void( event_source &, const point &, int )> mouse_pressed;

	/// @brief Action for mouse release events.
	///
	/// Callback action for mouse button release events.
	std::function<void( event_source &, const point &, int )> mouse_released;

	/// @brief Actionfor mouse motion events.
	///
	/// Callback action for mouse motion events.
	std::function<void( event_source &, const point & )> mouse_moved;

	/// @brief Action for mouse wheel events.
	///
	/// Callback action for mouse wheel events.
	std::function<void( event_source &, int )> mouse_wheel;

	/// @brief Action for key press events.
	///
	/// Callback action for key press events.
	std::function<void( event_source &, scancode )> key_pressed;

	/// @brief Action for key release events.
	///
	/// Callback action for key release events.
	std::function<void( event_source &, scancode )> key_released;

	/// @brief Action for text entered events.
	///
	/// Callback action for text entered events.
	std::function<void( event_source &, char32_t )> text_entered;

	std::function<bool( bool )> closed;
	std::function<void( void )> shown;
	std::function<void( void )> hidden;
	std::function<void( void )> minimized;
	std::function<void( void )> maximized;
	std::function<void( void )> restored;
	std::function<void( void )> exposed;
	std::function<void( coord_type, coord_type )> moved;
	std::function<void( coord_type, coord_type )> resized;
	std::function<void( void )> entered;
	std::function<void( void )> exited;

	virtual coord_type width( void ) = 0;
	virtual coord_type height( void ) = 0;

protected:
	virtual void make_current( const std::shared_ptr<cursor> & ) = 0;

	virtual void expose_event( coord_type x, coord_type y, coord_type w, coord_type h ) = 0;
	virtual void move_event( coord_type x, coord_type y ) = 0;
	virtual void resize_event( coord_type w, coord_type h ) = 0;

private:
	std::stack< std::shared_ptr<cursor> > _cursors;
	std::shared_ptr<cursor> _default_cursor;
};

////////////////////////////////////////

}

