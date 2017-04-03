
#include <iostream>
#include "keyboard.h"
#include <map>
#include <base/pointer.h>
#include <AppKit/AppKit.h>

////////////////////////////////////////

namespace
{

////////////////////////////////////////

const std::map<unsigned short,platform::scancode> keycode2scancode =
{
	{ 0x00, platform::scancode::KEY_A },
	{ 0x0b, platform::scancode::KEY_B },
	{ 0x08, platform::scancode::KEY_C },
	{ 0x02, platform::scancode::KEY_D },
	{ 0x0e, platform::scancode::KEY_E },
	{ 0x03, platform::scancode::KEY_F },
	{ 0x05, platform::scancode::KEY_G },
	{ 0x04, platform::scancode::KEY_H },
	{ 0x22, platform::scancode::KEY_I },
	{ 0x26, platform::scancode::KEY_J },
	{ 0x28, platform::scancode::KEY_K },
	{ 0x25, platform::scancode::KEY_L },
	{ 0x2e, platform::scancode::KEY_M },
	{ 0x2d, platform::scancode::KEY_N },
	{ 0x1f, platform::scancode::KEY_O },
	{ 0x23, platform::scancode::KEY_P },
	{ 0x0c, platform::scancode::KEY_Q },
	{ 0x0f, platform::scancode::KEY_R },
	{ 0x01, platform::scancode::KEY_S },
	{ 0x11, platform::scancode::KEY_T },
	{ 0x20, platform::scancode::KEY_U },
	{ 0x09, platform::scancode::KEY_V },
	{ 0x0d, platform::scancode::KEY_W },
	{ 0x07, platform::scancode::KEY_X },
	{ 0x10, platform::scancode::KEY_Y },
	{ 0x06, platform::scancode::KEY_Z },

	{ 0x12, platform::scancode::KEY_KP_1 },
	{ 0x13, platform::scancode::KEY_KP_2 },
	{ 0x14, platform::scancode::KEY_KP_3 },
	{ 0x15, platform::scancode::KEY_KP_4 },
	{ 0x17, platform::scancode::KEY_KP_5 },
	{ 0x16, platform::scancode::KEY_KP_6 },
	{ 0x1a, platform::scancode::KEY_KP_7 },
	{ 0x1c, platform::scancode::KEY_KP_8 },
	{ 0x19, platform::scancode::KEY_KP_9 },
	{ 0x1d, platform::scancode::KEY_KP_0 },

	{ 0x35, platform::scancode::KEY_ESCAPE },
	{ 0x3b, platform::scancode::KEY_LCTRL },
	{ 0x38, platform::scancode::KEY_LSHIFT },
	{ 0x3a, platform::scancode::KEY_LALT },
	{ 0x37, platform::scancode::KEY_LGUI },
	{ 0x3e, platform::scancode::KEY_RCTRL },
	{ 0x3c, platform::scancode::KEY_RSHIFT },
	{ 0x3d, platform::scancode::KEY_RALT },
	{ 0x36, platform::scancode::KEY_RGUI },
	{ 0x7f, platform::scancode::KEY_MENU },
	{ NSMenuFunctionKey, platform::scancode::KEY_MENU },
	{ 0x21, platform::scancode::KEY_LEFTBRACKET },
	{ 0x1e, platform::scancode::KEY_RIGHTBRACKET },
	{ 0x29, platform::scancode::KEY_SEMICOLON },
	{ 0x2b, platform::scancode::KEY_COMMA },
	{ 0x41, platform::scancode::KEY_KP_PERIOD },
	{ 0x2f, platform::scancode::KEY_PERIOD },
//	{ 0x27, platform::scancode::KEY_QUOTE },
	{ 0x2c, platform::scancode::KEY_SLASH },
	{ 0x2a, platform::scancode::KEY_BACKSLASH },

//	{ 0x0a, platform::scancode::KEY_TILDE },

	{ 0x51, platform::scancode::KEY_KP_EQUAL },
	{ 0x18, platform::scancode::KEY_EQUAL },
	{ 0x32, platform::scancode::KEY_MINUS },
	{ 0x31, platform::scancode::KEY_SPACE },
	{ 0x4c, platform::scancode::KEY_KP_ENTER },
	{ 0x24, platform::scancode::KEY_RETURN },
	{ 0x33, platform::scancode::KEY_BACKSPACE },
	{ 0x30, platform::scancode::KEY_TAB },

	{ 0x74, platform::scancode::KEY_PAGEUP },
	{ 0x79, platform::scancode::KEY_PAGEDOWN },
	{ 0x77, platform::scancode::KEY_END },
	{ 0x73, platform::scancode::KEY_HOME },

	{ NSPageUpFunctionKey, platform::scancode::KEY_PAGEUP },
	{ NSPageDownFunctionKey, platform::scancode::KEY_PAGEDOWN },
	{ NSEndFunctionKey, platform::scancode::KEY_END },
	{ NSHomeFunctionKey, platform::scancode::KEY_HOME },

	{ 0x72, platform::scancode::KEY_INSERT },
	{ NSInsertFunctionKey, platform::scancode::KEY_INSERT },
	{ 0x75, platform::scancode::KEY_DELETE },
	{ NSDeleteFunctionKey, platform::scancode::KEY_DELETE },

	{ 0x45, platform::scancode::KEY_KP_PLUS },
	{ 0x4e, platform::scancode::KEY_KP_MINUS },
	{ 0x43, platform::scancode::KEY_KP_MULTIPLY },
	{ 0x4b, platform::scancode::KEY_KP_DIVIDE },

	{ 0x7b, platform::scancode::KEY_LEFT },
	{ 0x7c, platform::scancode::KEY_RIGHT },
	{ 0x7e, platform::scancode::KEY_UP },
	{ 0x7d, platform::scancode::KEY_DOWN },

	{ NSLeftArrowFunctionKey, platform::scancode::KEY_LEFT },
	{ NSRightArrowFunctionKey, platform::scancode::KEY_RIGHT },
	{ NSUpArrowFunctionKey, platform::scancode::KEY_UP },
	{ NSDownArrowFunctionKey, platform::scancode::KEY_DOWN },

	{ 0x52, platform::scancode::KEY_KP_0 },
	{ 0x53, platform::scancode::KEY_KP_1 },
	{ 0x54, platform::scancode::KEY_KP_2 },
	{ 0x55, platform::scancode::KEY_KP_3 },
	{ 0x56, platform::scancode::KEY_KP_4 },
	{ 0x57, platform::scancode::KEY_KP_5 },
	{ 0x58, platform::scancode::KEY_KP_6 },
	{ 0x59, platform::scancode::KEY_KP_7 },
	{ 0x5b, platform::scancode::KEY_KP_8 },
	{ 0x5c, platform::scancode::KEY_KP_9 },

	{ 0x7a, platform::scancode::KEY_F1 },
	{ 0x78, platform::scancode::KEY_F2 },
	{ 0x63, platform::scancode::KEY_F3 },
	{ 0x76, platform::scancode::KEY_F4 },
	{ 0x60, platform::scancode::KEY_F5 },
	{ 0x61, platform::scancode::KEY_F6 },
	{ 0x62, platform::scancode::KEY_F7 },
	{ 0x64, platform::scancode::KEY_F8 },
	{ 0x65, platform::scancode::KEY_F9 },
	{ 0x6d, platform::scancode::KEY_F10 },
	{ 0x67, platform::scancode::KEY_F11 },
	{ 0x6f, platform::scancode::KEY_F12 },
	{ 0x69, platform::scancode::KEY_F13 },
	{ 0x6b, platform::scancode::KEY_F14 },
	{ 0x71, platform::scancode::KEY_F15 },

	{ NSF1FunctionKey, platform::scancode::KEY_F1 },
	{ NSF2FunctionKey, platform::scancode::KEY_F2 },
	{ NSF3FunctionKey, platform::scancode::KEY_F3 },
	{ NSF4FunctionKey, platform::scancode::KEY_F4 },
	{ NSF5FunctionKey, platform::scancode::KEY_F5 },
	{ NSF6FunctionKey, platform::scancode::KEY_F6 },
	{ NSF7FunctionKey, platform::scancode::KEY_F7 },
	{ NSF8FunctionKey, platform::scancode::KEY_F8 },
	{ NSF9FunctionKey, platform::scancode::KEY_F9 },
	{ NSF10FunctionKey, platform::scancode::KEY_F10 },
	{ NSF11FunctionKey, platform::scancode::KEY_F11 },
	{ NSF12FunctionKey, platform::scancode::KEY_F12 },
	{ NSF13FunctionKey, platform::scancode::KEY_F13 },
	{ NSF14FunctionKey, platform::scancode::KEY_F14 },
	{ NSF15FunctionKey, platform::scancode::KEY_F15 },

	{ NSPauseFunctionKey, platform::scancode::KEY_PAUSE },

};

////////////////////////////////////////

}

////////////////////////////////////////

namespace platform { namespace cocoa
{

////////////////////////////////////////

keyboard::keyboard( void )
{
	update_mapping();
}

////////////////////////////////////////

keyboard::~keyboard( void )
{
}

////////////////////////////////////////

void keyboard::update_mapping( void )
{
}

////////////////////////////////////////

void keyboard::start( void )
{
}

////////////////////////////////////////

void keyboard::cancel( void )
{
}

////////////////////////////////////////

void keyboard::shutdown( void )
{
}

////////////////////////////////////////

intptr_t keyboard::poll_object( void )
{
	return intptr_t(-1);
}

////////////////////////////////////////

bool keyboard::poll_timeout( duration &, const time_point & )
{
	return false;
}

////////////////////////////////////////

void keyboard::emit( const time_point &curtime )
{
}

////////////////////////////////////////

platform::scancode keyboard::get_scancode( unsigned short key )
{
	auto c = keycode2scancode.find( key );
	if ( c != keycode2scancode.end() )
		return c->second;

	return platform::scancode::KEY_UNKNOWN;
}

////////////////////////////////////////

} }

