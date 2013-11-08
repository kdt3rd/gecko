
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <draw/canvas.h>
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

//	virtual void lower( void ) = 0;

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

//	virtual void set_icon( const icon &i );

	virtual std::shared_ptr<draw::canvas> canvas( void ) = 0;

	void when_closed( std::function<void(void)> f ) { _closed = f; }
	void when_shown( std::function<void(void)> f ) { _shown = f; }
	void when_hidden( std::function<void(void)> f ) { _hidden = f; }
	void when_minimized( std::function<void(void)> f ) { _minimized = f; }
	void when_maximized( std::function<void(void)> f ) { _maximized = f; }
	void when_restored( std::function<void(void)> f ) { _restored = f; }
	void when_exposed( std::function<void()> f ) { _exposed = f; }
	void when_moved( std::function<void(double,double)> f ) { _moved = f; }
	void when_resized( std::function<void(double,double)> f ) { _resized = f; }
	void when_entered( std::function<void(void)> f ) { _entered = f; }
	void when_exited( std::function<void(void)> f ) { _exited = f; }

	/// @brief Set the callback for mouse press events.
	///
	/// Set the callback function for mouse button press events.
	/// @param f Function to call
	void when_mouse_pressed( std::function<void(const std::shared_ptr<mouse>&,int)> f ) { _mouse_pressed = f; }

	/// @brief Set the callback for mouse release events.
	///
	/// Set the callback function for mouse button release events.
	/// @param f Function to call
	void when_mouse_released( std::function<void(const std::shared_ptr<mouse>&,int)> f ) { _mouse_released = f; }

	/// @brief Set the callback for mouse motion events.
	///
	/// Set the callback function for mouse motion events.
	/// @param f Function to call
	void when_mouse_moved( std::function<void(const std::shared_ptr<mouse>&,const draw::point &)> f ) { _mouse_moved = f; }

	/// @brief Set the callback for key press events.
	///
	/// Set the callback function for key press events.
	/// @param f Function to call
	void when_key_pressed( std::function<void(const std::shared_ptr<keyboard>&,scancode)> f ) { _key_pressed = f; }

	/// @brief Set the callback for key release events.
	///
	/// Set the callback function for key release events.
	/// @param f Function to call
	void when_key_released( std::function<void(const std::shared_ptr<keyboard>&,scancode)> f ) { _key_released = f; }

	void closed( void ) { if ( _closed ) _closed(); }
	void shown( void ) { if ( _shown ) _shown(); }
	void hidden( void ) { if ( _hidden ) _hidden(); }
	void minimized( void ) { if ( _minimized ) _minimized(); }
	void maximized( void ) { if ( _maximized ) _maximized(); }
	void restored( void ) { if ( _restored ) _restored(); }
	virtual void exposed( void ) { if ( _exposed ) _exposed(); }
	void moved( double x, double y ) { if ( _moved ) _moved( x, y ); }
	virtual void resized( double w, double h ) { if ( _resized ) _resized( w, h ); }
	void entered( void ) { if ( _entered ) _entered(); }
	void exited( void ) { if ( _exited ) _exited(); }

	/// @brief Send a mouse press event.
	///
	/// Sends a mouse button press event of the given button.
	/// @param w Window in which the press event occurred
	/// @param button Button which was pressed
	void mouse_pressed( const std::shared_ptr<mouse> &w, int button ) { if ( _mouse_pressed ) _mouse_pressed( w, button ); }

	/// @brief Send a mouse release event.
	///
	/// Sends a mouse button release event of the given button.
	/// @param w Window in which the release event occurred
	/// @param button Button which was released
	void mouse_released( const std::shared_ptr<mouse> &w, int button ) { if ( _mouse_released ) _mouse_released( w, button ); }

	/// @brief Send a mouse motion event.
	///
	/// Send a mouse motion event.
	/// @param w Window in which the mouse moved
	/// @param x New x location of the mouse
	/// @param y New y location of the mouse
	void mouse_moved( const std::shared_ptr<mouse> &w, const draw::point &p ) { if ( _mouse_moved ) _mouse_moved( w, p ); }

	/// @brief Send a key press event.
	///
	/// Send a key press event with the given scan code.
	/// @param sc Key which was pressed
	void key_pressed( const std::shared_ptr<keyboard> &k, scancode sc ) { if ( _key_pressed ) _key_pressed( k, sc ); }

	/// @brief Send a key release event.
	///
	/// Send a key release event with the given scan code.
	/// @param sc Key which was release
	void key_released( const std::shared_ptr<keyboard> &k, scancode sc ) { if ( _key_released ) _key_released( k, sc ); }


private:
	std::function<void(void)> _closed;
	std::function<void(void)> _shown;
	std::function<void(void)> _hidden;
	std::function<void(void)> _minimized;
	std::function<void(void)> _maximized;
	std::function<void(void)> _restored;
	std::function<void(void)> _exposed;
	std::function<void(double,double)> _moved;
	std::function<void(double,double)> _resized;
	std::function<void(void)> _entered;
	std::function<void(void)> _exited;

	std::function<void(const std::shared_ptr<mouse>&,const draw::point&)> _mouse_moved;
	std::function<void(const std::shared_ptr<mouse>&,int)> _mouse_pressed;
	std::function<void(const std::shared_ptr<mouse>&,int)> _mouse_released;

	std::function<void(const std::shared_ptr<keyboard>&,scancode)> _key_pressed;
	std::function<void(const std::shared_ptr<keyboard>&,scancode)> _key_released;
};

////////////////////////////////////////

}

