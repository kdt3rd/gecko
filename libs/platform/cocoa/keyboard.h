
#pragma once

#include <platform/keyboard.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of keyboard.
class keyboard : public ::platform::keyboard
{
public:
	keyboard( void );
	~keyboard( void );

	void update_mapping( void );

//	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );
	::platform::scancode get_scancode( unsigned short code );

private:
};

////////////////////////////////////////

} }

