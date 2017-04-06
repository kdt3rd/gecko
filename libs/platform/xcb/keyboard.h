//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/keyboard.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

namespace platform { namespace xcb
{

////////////////////////////////////////

/// @brief XCB implementation of platform::keyboard.
class keyboard : public platform::keyboard
{
public:
	/// @brief Constructor.
	keyboard( xcb_connection_t *c );
	~keyboard( void );

	/// @brief Update key mapping.
	///
	/// Updating key mapping when the user changes the keymap.
	void update_mapping( void );

	/// @brief Get keysym of the given keycode.
	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );

	/// @brief Get the platform::scancode of the given keycode.
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

} }

