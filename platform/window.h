
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <draw/canvas.h>
#include <core/action.h>
#include <gl/context.h>
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

	/// @brief Resize the window.
	///
	/// Resize the window to the given size.
	/// @param w New width of the window
	/// @param h New height of the window
	virtual void resize( double w, double h ) = 0;

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

	virtual void invalidate( const draw::rect &r ) = 0;

//	virtual void set_icon( const icon &i );

	virtual gl::context context( void ) = 0;
	virtual std::shared_ptr<draw::canvas> canvas( void ) = 0;

	/// @brief Action for mouse press events.
	///
	/// Callback action for mouse button press events.
	action<void( const std::shared_ptr<mouse> &, const draw::point &, int )> mouse_pressed;

	/// @brief Action for mouse release events.
	///
	/// Callback action for mouse button release events.
	action<void( const std::shared_ptr<mouse> &, const draw::point &, int )> mouse_released;

	/// @brief Actionfor mouse motion events.
	///
	/// Callback action for mouse motion events.
	action<void( const std::shared_ptr<mouse> &, const draw::point & )> mouse_moved;

	/// @brief Action for key press events.
	///
	/// Callback action for key press events.
	action<void( const std::shared_ptr<keyboard> &, scancode )> key_pressed;

	/// @brief Action for key release events.
	///
	/// Callback action for key release events.
	action<void( const std::shared_ptr<keyboard> &, scancode )> key_released;

	/// @brief Action for text entered events.
	///
	/// Callback action for text entered events.
	action<void( const std::shared_ptr<keyboard> &, char32_t )> text_entered;

	action<void( void )> closed;
	action<void( void )> shown;
	action<void( void )> hidden;
	action<void( void )> minimized;
	action<void( void )> maximized;
	action<void( void )> restored;
	action<void( void )> exposed;
	action<void( double, double )> moved;
	action<void( double, double )> resized;
	action<void( void )> entered;
	action<void( void )> exited;

	virtual double width( void ) = 0;
	virtual double height( void ) = 0;
};

////////////////////////////////////////

}

