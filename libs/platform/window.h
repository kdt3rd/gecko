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
#include <base/rect.h>
#include "mouse.h"
#include "keyboard.h"

namespace platform
{

////////////////////////////////////////

/// @brief A rectangular area of the screen.
class window
{
public:
	/// @brief Constructor.
	window( void );

	/// @brief Destructor.
	virtual ~window( void );

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

//	virtual rect geometry( void ) = 0;

//	virtual void set_geometry( const rect &r ) = 0;

	/// @brief Move the window.
	///
	/// Move the window to the given position.
	/// @param x New x position of the window
	/// @param y New y position of the window
	virtual void move( double x, double y ) = 0;
	void move( const base::point &p ) { move( p.x(), p.y() ); }

	/// @brief Resize the window.
	///
	/// Resize the window to the given size.
	/// @param w New width of the window
	/// @param h New height of the window
	virtual void resize( double w, double h ) = 0;
	void resize( const base::size &s ) { resize( s.w(), s.h() ); }

	/// @brief Set minimum window size.
	///
	/// The window will not be allowed to resize smaller than the minimum given.
	/// @param w Minimum width for the window
	/// @param h Minimum height for the window
	virtual void set_minimum_size( double w, double h ) = 0;

	/// @brief Set the window title.
	///
	/// Set the window title shown in the title bar.
	/// @param t The window title
	virtual void set_title( const std::string &t ) = 0;

	virtual void invalidate( const base::rect &r ) = 0;

//	virtual void set_icon( const icon &i );

	virtual void acquire( void ) = 0;
	virtual void release( void ) = 0;

	/// @brief Action for mouse press events.
	///
	/// Callback action for mouse button press events.
	std::function<void( const std::shared_ptr<mouse> &, const base::point &, int )> mouse_pressed;

	/// @brief Action for mouse release events.
	///
	/// Callback action for mouse button release events.
	std::function<void( const std::shared_ptr<mouse> &, const base::point &, int )> mouse_released;

	/// @brief Actionfor mouse motion events.
	///
	/// Callback action for mouse motion events.
	std::function<void( const std::shared_ptr<mouse> &, const base::point & )> mouse_moved;

	/// @brief Action for mouse wheel events.
	///
	/// Callback action for mouse wheel events.
	std::function<void( const std::shared_ptr<mouse> &, int )> mouse_wheel;

	/// @brief Action for key press events.
	///
	/// Callback action for key press events.
	std::function<void( const std::shared_ptr<keyboard> &, scancode )> key_pressed;

	/// @brief Action for key release events.
	///
	/// Callback action for key release events.
	std::function<void( const std::shared_ptr<keyboard> &, scancode )> key_released;

	/// @brief Action for text entered events.
	///
	/// Callback action for text entered events.
	std::function<void( const std::shared_ptr<keyboard> &, char32_t )> text_entered;

	std::function<void( void )> closed;
	std::function<void( void )> shown;
	std::function<void( void )> hidden;
	std::function<void( void )> minimized;
	std::function<void( void )> maximized;
	std::function<void( void )> restored;
	std::function<void( void )> exposed;
	std::function<void( double, double )> moved;
	std::function<void( double, double )> resized;
	std::function<void( void )> entered;
	std::function<void( void )> exited;

	virtual double width( void ) = 0;
	virtual double height( void ) = 0;
};

////////////////////////////////////////

}

