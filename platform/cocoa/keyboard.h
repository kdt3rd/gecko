
#pragma once

#include <platform/keyboard.h>

namespace cocoa
{

////////////////////////////////////////

class keyboard : public platform::keyboard
{
public:
	keyboard( void );
	virtual ~keyboard( void );

	void update_mapping( void );

//	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );
//	platform::scancode get_scancode( xcb_keycode_t code );

private:
};

////////////////////////////////////////

}
