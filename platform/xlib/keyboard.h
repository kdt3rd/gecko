
#pragma once

#include <platform/keyboard.h>
#include <X11/Xlib.h>

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::keyboard.
class keyboard : public platform::keyboard
{
public:
	/// @brief Constructor.
	keyboard( void );
	~keyboard( void );

	/// @brief Get the platform::scancode of the given keycode.
	platform::scancode get_scancode( XKeyEvent &ev );
};

////////////////////////////////////////

}

