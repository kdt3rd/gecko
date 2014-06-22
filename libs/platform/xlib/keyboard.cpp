
#include <iostream>
#include "keyboard.h"
#include <map>
#include <core/pointer.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

////////////////////////////////////////

namespace
{

////////////////////////////////////////

const std::map<KeySym,platform::scancode> sym2code =
{
	{ XK_Return, platform::scancode::KEY_RETURN },
	{ XK_Escape, platform::scancode::KEY_ESCAPE },
	{ XK_BackSpace, platform::scancode::KEY_BACKSPACE },
	{ XK_Tab, platform::scancode::KEY_TAB },
	{ XK_space, platform::scancode::KEY_SPACE },
	{ XK_minus, platform::scancode::KEY_MINUS },
	{ XK_equal, platform::scancode::KEY_EQUAL },
	{ XK_bracketleft, platform::scancode::KEY_LEFTBRACKET },
	{ XK_bracketright, platform::scancode::KEY_RIGHTBRACKET },
	{ XK_slash, platform::scancode::KEY_BACKSLASH },
	{ XK_semicolon, platform::scancode::KEY_SEMICOLON },
	{ XK_apostrophe, platform::scancode::KEY_APOSTROPHE },
	{ XK_grave, platform::scancode::KEY_GRAVE },
	{ XK_comma, platform::scancode::KEY_COMMA },
	{ XK_period, platform::scancode::KEY_PERIOD },
	{ XK_slash, platform::scancode::KEY_SLASH },
	{ XK_Caps_Lock, platform::scancode::KEY_CAPSLOCK },
	{ XK_F1, platform::scancode::KEY_F1 },
	{ XK_F2, platform::scancode::KEY_F2 },
	{ XK_F3, platform::scancode::KEY_F3 },
	{ XK_F4, platform::scancode::KEY_F4 },
	{ XK_F5, platform::scancode::KEY_F5 },
	{ XK_F6, platform::scancode::KEY_F6 },
	{ XK_F7, platform::scancode::KEY_F7 },
	{ XK_F8, platform::scancode::KEY_F8 },
	{ XK_F9, platform::scancode::KEY_F9 },
	{ XK_F10, platform::scancode::KEY_F10 },
	{ XK_F11, platform::scancode::KEY_F11 },
	{ XK_F12, platform::scancode::KEY_F12 },
	{ XK_Sys_Req, platform::scancode::KEY_PRINTSCREEN },
	{ XK_Scroll_Lock, platform::scancode::KEY_SCROLLLOCK},
	{ XK_Pause, platform::scancode::KEY_PAUSE },
	{ XK_Insert, platform::scancode::KEY_INSERT},
	{ XK_Home, platform::scancode::KEY_HOME},
	{ XK_Page_Up, platform::scancode::KEY_PAGEUP},
	{ XK_Delete, platform::scancode::KEY_DELETE},
	{ XK_End, platform::scancode::KEY_END },
	{ XK_Page_Down, platform::scancode::KEY_PAGEDOWN},
	{ XK_Right, platform::scancode::KEY_RIGHT },
	{ XK_Left, platform::scancode::KEY_LEFT},
	{ XK_Down, platform::scancode::KEY_DOWN},
	{ XK_Up, platform::scancode::KEY_UP},
	{ XK_Num_Lock, platform::scancode::KEY_NUMLOCK },
	{ XK_KP_Divide, platform::scancode::KEY_KP_DIVIDE },
	{ XK_KP_Multiply, platform::scancode::KEY_KP_MULTIPLY },
	{ XK_KP_Subtract, platform::scancode::KEY_KP_MINUS},
	{ XK_KP_Add, platform::scancode::KEY_KP_PLUS },
	{ XK_KP_Enter, platform::scancode::KEY_KP_ENTER},
	{ XK_KP_End, platform::scancode::KEY_KP_1 },
	{ XK_KP_Down, platform::scancode::KEY_KP_2 },
	{ XK_KP_Page_Down, platform::scancode::KEY_KP_3 },
	{ XK_KP_Left, platform::scancode::KEY_KP_4 },
	{ XK_KP_Begin, platform::scancode::KEY_KP_5 },
	{ XK_KP_Right, platform::scancode::KEY_KP_6 },
	{ XK_KP_Home, platform::scancode::KEY_KP_7 },
	{ XK_KP_Up, platform::scancode::KEY_KP_8 },
	{ XK_KP_Page_Up, platform::scancode::KEY_KP_9 },
	{ XK_KP_Insert, platform::scancode::KEY_KP_0 },
	{ XK_KP_Delete, platform::scancode::KEY_KP_PERIOD },
//	{ XK_, platform::scancode::KEY_APPLICATION },
//	{ XK_, platform::scancode::KEY_POWER },
	{ XK_KP_Equal, platform::scancode::KEY_KP_EQUAL},
	{ XK_F13, platform::scancode::KEY_F13 },
	{ XK_F14, platform::scancode::KEY_F14 },
	{ XK_F15, platform::scancode::KEY_F15 },
	{ XK_F16, platform::scancode::KEY_F16 },
	{ XK_F17, platform::scancode::KEY_F17 },
	{ XK_F18, platform::scancode::KEY_F18 },
	{ XK_F19, platform::scancode::KEY_F19 },
	{ XK_F20, platform::scancode::KEY_F20 },
	{ XK_F21, platform::scancode::KEY_F21 },
	{ XK_F22, platform::scancode::KEY_F22 },
	{ XK_F23, platform::scancode::KEY_F23 },
	{ XK_F24, platform::scancode::KEY_F24 },
//	{ XK_Execute, platform::scancode::KEY_EXECUTE},
	{ XK_Help, platform::scancode::KEY_HELP },
	{ XK_Menu, platform::scancode::KEY_MENU },
	{ XK_Select, platform::scancode::KEY_SELECT },
	{ XK_Cancel, platform::scancode::KEY_STOP },
	{ XK_Redo, platform::scancode::KEY_AGAIN},
	{ XK_Undo, platform::scancode::KEY_UNDO },
//	{ XK_, platform::scancode::KEY_CUT},
//	{ XK_, platform::scancode::KEY_COPY },
//	{ XK_, platform::scancode::KEY_PASTE},
	{ XK_Find, platform::scancode::KEY_FIND },
//	{ XK_, platform::scancode::KEY_MUTE },
//	{ XK_, platform::scancode::KEY_VOLUMEUP },
//	{ XK_, platform::scancode::KEY_VOLUMEDOWN },
	{ XK_Control_L, platform::scancode::KEY_LCTRL },
	{ XK_Shift_L, platform::scancode::KEY_LSHIFT },
	{ XK_Alt_L, platform::scancode::KEY_LALT },
	{ XK_Meta_L, platform::scancode::KEY_LGUI },
	{ XK_Control_R, platform::scancode::KEY_RCTRL},
	{ XK_Shift_R, platform::scancode::KEY_RSHIFT },
	{ XK_Alt_R, platform::scancode::KEY_RALT },
	{ XK_Meta_R, platform::scancode::KEY_RGUI },
};

////////////////////////////////////////

}

////////////////////////////////////////

namespace xlib
{

////////////////////////////////////////

keyboard::keyboard( void )
{
}

////////////////////////////////////////

keyboard::~keyboard( void )
{
}

////////////////////////////////////////

platform::scancode keyboard::get_scancode( XKeyEvent &ev )
{
	KeySym symbol;
	char buffer[32];
	XLookupString( &ev, buffer, sizeof(buffer), &symbol, nullptr );

	if ( symbol >= XK_a && symbol <= XK_z )
		return static_cast<platform::scancode>( static_cast<uint32_t>(platform::scancode::KEY_A) + ( symbol - XK_a ) );

	if ( symbol >= XK_0 && symbol <= XK_9 )
		return static_cast<platform::scancode>( static_cast<uint32_t>(platform::scancode::KEY_0) + ( symbol - XK_0 ) );

	auto c = sym2code.find( symbol );
	if ( c != sym2code.end() )
		return c->second;

	std::cout << "KEYSYM = " << symbol << std::endl; 
	return platform::scancode::KEY_UNKNOWN;
}

////////////////////////////////////////

}

