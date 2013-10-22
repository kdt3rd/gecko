
#pragma once

#include <memory>
#include <functional>
#include "window.h"

namespace platform
{

////////////////////////////////////////

/// @brief Mouse device
/// Mouse device which can send button press/release and mouse motion events.
class mouse
{
public:

	/// @brief Constructor
	mouse( void );

	/// @brief Destructor
	virtual ~mouse( void );

	/// @brief Set the callback for press events
	/// Set the callback function for button press events.
	/// @param f Function to call
	void when_pressed( std::function<void(const std::shared_ptr<window>&,int)> f ) { _pressed = f; }

	/// @brief Set the callback for release events
	/// Set the callback function for button release events.
	/// @param f Function to call
	void when_released( std::function<void(const std::shared_ptr<window>&,int)> f ) { _released = f; }

	/// @brief Set the callback for motion events
	/// Set the callback function for mouse motion events.
	/// @param f Function to call
	void when_moved( std::function<void(const std::shared_ptr<window>&,double,double)> f ) { _moved = f; }

	/// @brief Send a press event
	/// Sends a mouse button press event of the given button.
	/// @param w Window in which the press event occurred
	/// @param button Button which was pressed
	void pressed( const std::shared_ptr<window> &w, int button ) { if ( _pressed ) _pressed( w, button ); }

	/// @brief Send a release event
	/// Sends a mouse button release event of the given button.
	/// @param w Window in which the release event occurred
	/// @param button Button which was released
	void released( const std::shared_ptr<window> &w, int button ) { if ( _released ) _released( w, button ); }

	/// @brief Send a motion event
	/// Send a mouse motion event.
	/// @param w Window in which the mouse moved
	/// @param x New x location of the mouse
	/// @param y New y location of the mouse
	void moved( const std::shared_ptr<window> &w, double x, double y ) { if ( _moved ) _moved( w, x, y ); }

private:
	std::function<void(const std::shared_ptr<window>&,double,double)> _moved;
	std::function<void(const std::shared_ptr<window>&,int)> _pressed;
	std::function<void(const std::shared_ptr<window>&,int)> _released;
};

////////////////////////////////////////

}

