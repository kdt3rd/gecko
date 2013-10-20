
#pragma once

#include <platform/keyboard.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

namespace xcb
{

////////////////////////////////////////

class keyboard : public platform::keyboard
{
public:
	keyboard( xcb_connection_t *c );
	virtual ~keyboard( void );

	void update_mapping( void );

	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );
	platform::scancode get_scancode( xcb_keycode_t code );

private:
	xcb_connection_t *_connection;
	xcb_key_symbols_t *_keysyms = nullptr;
	uint16_t _numlock = 0;
	uint16_t _shiftlock = 0;
	uint16_t _capslock = 0;
	uint16_t _modeswitch = 0;
};

////////////////////////////////////////

}

