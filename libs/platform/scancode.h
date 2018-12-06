//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

struct modifier
{
	static const uint8_t LEFT_CTRL = 0x01;
	static const uint8_t LEFT_SHIFT = 0x02;
	static const uint8_t LEFT_ALT = 0x04;
	static const uint8_t LEFT_META = 0x08;
	static const uint8_t RIGHT_CTRL = 0x10;
	static const uint8_t RIGHT_SHIFT = 0x20;
	static const uint8_t RIGHT_ALT = 0x40;
	static const uint8_t RIGHT_META = 0x80;
};

////////////////////////////////////////

/// @brief Scan codes for keys.
///
/// This enum is not all possible *characters*, but rather corresponds
/// to the USB scan codes used for physical keys (i.e. ignores shift /
/// fn modifiers)
///
/// NB: this does not imply any particular layout and only applies to
/// keyboard events
/// 
/// http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
///
/// section 10, Keyboard / Keypad page (0x07)
///
/// NB: comments are for a typical U.S. keyboard and will be different
/// for different languages
///
enum class scancode : uint8_t
{
	KEY_NO_EVENT = 0,         ///< reserved (no event indicated)
	KEY_ERROR_ROLLOVER = 1,   ///< ErrorRollOver
	KEY_ERROR_POSTFAIL = 2,   ///< POSTFail
	KEY_ERROR_UNDEFINED = 3,  ///< ErrorUndefined
	KEY_UNKNOWN = KEY_ERROR_UNDEFINED,
	KEY_A = 4, ///< a and A
	KEY_B = 5, ///< b and B
	KEY_C = 6, ///< c and C
	KEY_D = 7, ///< d and D
	KEY_E = 8, ///< e and E
	KEY_F = 9, ///< f and F
	KEY_G = 10, ///< g and G
	KEY_H = 11, ///< h and H
	KEY_I = 12, ///< i and I
	KEY_J = 13, ///< j and J
	KEY_K = 14, ///< k and K
	KEY_L = 15, ///< l and L
	KEY_M = 16, ///< m and M
	KEY_N = 17, ///< n and N
	KEY_O = 18, ///< o and O
	KEY_P = 19, ///< p and P
	KEY_Q = 20, ///< q and Q
	KEY_R = 21, ///< r and R
	KEY_S = 22, ///< s and S
	KEY_T = 23, ///< t and T
	KEY_U = 24, ///< u and U
	KEY_V = 25, ///< v and V
	KEY_W = 26, ///< w and W
	KEY_X = 27, ///< x and X
	KEY_Y = 28, ///< y and Y
	KEY_Z = 29, ///< z and Z

	KEY_1 = 30, ///< 1 and !
	KEY_2 = 31, ///< 2 and @
	KEY_3 = 32, ///< 3 and #
	KEY_4 = 33, ///< 4 and $
	KEY_5 = 34, ///< 5 and %
	KEY_6 = 35, ///< 6 and ^
	KEY_7 = 36, ///< 7 and &
	KEY_8 = 37, ///< 8 and *
	KEY_9 = 38, ///< 9 and (
	KEY_0 = 39, ///< 0 and )

	KEY_RETURN       = 40, ///< Return (ENTER)
	KEY_ESCAPE       = 41, ///< ESCAPE
	KEY_BACKSPACE    = 42, ///< DELETE (Backspace)
	KEY_TAB          = 43, ///< Tab
	KEY_SPACE        = 44, ///< Spacebar
	KEY_MINUS        = 45, ///< - and _
	KEY_EQUAL        = 46, ///< = and +
	KEY_LEFTBRACKET  = 47, ///< [ and {
	KEY_RIGHTBRACKET = 48, ///< ] and }
	KEY_BACKSLASH    = 49, ///< \ and |
	KEY_NONUSLASH    = 50, ///< Non-US # and ~
	KEY_SEMICOLON    = 51, ///< ; and :
	KEY_APOSTROPHE   = 52, ///< ' and "
	KEY_GRAVE        = 53, ///< Grave Accent and Tilde
	KEY_COMMA        = 54, ///< , and <
	KEY_PERIOD       = 55, ///< . and >
	KEY_SLASH        = 56, ///< / and ?
	KEY_CAPSLOCK     = 57, ///< Caps Lock
	
	KEY_F1  = 58, ///< F1
	KEY_F2  = 59, ///< F2
	KEY_F3  = 60, ///< F3
	KEY_F4  = 61, ///< F4
	KEY_F5  = 62, ///< F5
	KEY_F6  = 63, ///< F6
	KEY_F7  = 64, ///< F7
	KEY_F8  = 65, ///< F8
	KEY_F9  = 66, ///< F9
	KEY_F10 = 67, ///< F10
	KEY_F11 = 68, ///< F11
	KEY_F12 = 69, ///< F12

	KEY_PRINTSCREEN = 70, ///< PrintScreen
	KEY_SCROLLLOCK  = 71, ///< Scroll Lock
	KEY_PAUSE       = 72, ///< Pause
	KEY_INSERT      = 73, ///< Insert
	KEY_HOME        = 74, ///< Home
	KEY_PAGEUP      = 75, ///< PageUp
	KEY_DELETE      = 76, ///< Delete Forward
	KEY_END         = 77, ///< End
	KEY_PAGEDOWN    = 78, ///< PageDown
	KEY_RIGHT       = 79, ///< RightArrow
	KEY_LEFT        = 80, ///< LeftArrow
	KEY_DOWN        = 81, ///< DownArrow
	KEY_UP          = 82, ///< UpArrow

	KEY_NUMLOCK     = 83, ///< Num Lock and Clear
	KEY_KP_DIVIDE   = 84, ///< Keypad /
	KEY_KP_MULTIPLY = 85, ///< Keypad *
	KEY_KP_MINUS    = 86, ///< Keypad -
	KEY_KP_PLUS     = 87, ///< Keypad +
	KEY_KP_ENTER    = 88, ///< Keypad ENTER
	KEY_KP_1        = 89, ///< 1 and End
	KEY_KP_2        = 90, ///< 2 and Down Arrow
	KEY_KP_3        = 91, ///< 3 and PageDn
	KEY_KP_4        = 92, ///< 4 and Left Arrow
	KEY_KP_5        = 93, ///< 5
	KEY_KP_6        = 94, ///< 6 and Right Arrow
	KEY_KP_7        = 95, ///< 7 and Home
	KEY_KP_8        = 96, ///< 8 and Up Arrow
	KEY_KP_9        = 97, ///< 9 and PageUp
	KEY_KP_0        = 98, ///< 0 and Insert
	KEY_KP_PERIOD   = 99, ///< . and Delete

	KEY_NONUBACKSLASH = 100, ///< Non-US \ and |
	KEY_APPLICATION   = 101, ///< Application
	KEY_POWER         = 102, ///< Power
	KEY_KP_EQUAL      = 103, ///< Keypad =

	KEY_F13 = 104, ///< F13
	KEY_F14 = 105,
	KEY_F15 = 106,
	KEY_F16 = 107,
	KEY_F17 = 108,
	KEY_F18 = 109,
	KEY_F19 = 110,
	KEY_F20 = 111,
	KEY_F21 = 112,
	KEY_F22 = 113,
	KEY_F23 = 114,
	KEY_F24 = 115,

	KEY_EXEC       = 116,
	KEY_HELP       = 117,
	KEY_MENU       = 118,
	KEY_SELECT     = 119,
	KEY_STOP       = 120,
	KEY_AGAIN      = 121,
	KEY_UNDO       = 122,
	KEY_CUT        = 123,
	KEY_COPY       = 124,
	KEY_PASTE      = 125,
	KEY_FIND       = 126,
	KEY_MUTE       = 127,
	KEY_VOLUMEUP   = 128,
	KEY_VOLUMEDOWN = 129,

	KEY_LOCKING_CAPSLOCK = 130,
	KEY_LOCKING_NUMLOCK = 131,
	KEY_LOCKING_SCROLLLOCK = 132,

	KEY_KP_COMMA = 133,
	KEY_KP_EQUAL2 = 134,

	KEY_INTERNATIONAL1 = 135, ///< Ro?
	KEY_INTERNATIONAL2 = 136, ///< Katakana / Hiragana?
	KEY_INTERNATIONAL3 = 137, ///< Yen?
	KEY_INTERNATIONAL4 = 138, ///< Henkan?
	KEY_INTERNATIONAL5 = 139, ///< MuHenkan?
	KEY_INTERNATIONAL6 = 140, ///< KP Japanese Comma?
	KEY_INTERNATIONAL7 = 141,
	KEY_INTERNATIONAL8 = 142,
	KEY_INTERNATIONAL9 = 143,
	KEY_LANG1 = 144, ///< Hangeul?
	KEY_LANG2 = 145, ///< Hanja?
	KEY_LANG3 = 146, ///< Katakana?
	KEY_LANG4 = 147, ///< Hiragana?
	KEY_LANG5 = 148, ///< ZenKaKuHanKaKu?
	KEY_LANG6 = 149,
	KEY_LANG7 = 150,
	KEY_LANG8 = 151,
	KEY_LANG9 = 152,

	KEY_ALT_ERASE = 153,
	KEY_SYSRQ = 154, ///< SysReq / Attention
	KEY_CANCEL = 155,
	KEY_CLEAR = 156,
	KEY_PRIOR = 157,
	KEY_RETURN2 = 158,
	KEY_SEPARATOR = 159,
	KEY_OUT = 160,
	KEY_OPER = 161,
	KEY_AGAIN2 = 162, ///< Clear / Again
	KEY_CRSEL = 163, ///< CrSel / Props
	KEY_EXSEL = 164,

	KEY_RESERVED1 = 165,
	KEY_RESERVED2 = 166,
	KEY_RESERVED3 = 167,
	KEY_RESERVED4 = 168,
	KEY_RESERVED5 = 169,
	KEY_RESERVED6 = 170,

	KEY_PLAY_NEXT_TRACK = 171,
	KEY_PLAY_TOGGLE = 172,
	KEY_PLAY_PREV_TRACK = 173,
	KEY_PLAY_STOP = 174,

	KEY_RESERVED7 = 175,

	KEY_KP_00               = 176,
	KEY_KP_000              = 177,
	KEY_KP_THOUSAND_SEP     = 178,
	KEY_KP_DECIMAL_SEP      = 179,
	KEY_KP_CURRENCY_UNIT    = 180,
	KEY_KP_CURRENCY_SUBUNIT = 181,

	KEY_KP_OPEN_PAREN  = 182,
	KEY_KP_CLOSE_PAREN = 183,
	KEY_KP_OPEN_BRACE  = 184,
	KEY_KP_CLOSE_BRACE = 185,
	KEY_KP_TAB         = 186,
	KEY_KP_BACKSPACE   = 187,
	KEY_KP_A           = 188,
	KEY_KP_B           = 189,
	KEY_KP_C           = 190,
	KEY_KP_D           = 191,
	KEY_KP_E           = 192,
	KEY_KP_F           = 193,
	KEY_KP_XOR         = 194, ///< XOR
	KEY_KP_HAT         = 195, ///< ^
	KEY_KP_PERCENT     = 196, ///< %
	KEY_KP_LESSTHAN    = 197,
	KEY_KP_GREATERTHAN = 198,
	KEY_KP_AMPERSAND   = 199, ///< &
	KEY_KP_DOUBLE_AMPERSAND = 200, ///< &&
	KEY_KP_PIPE        = 201,///< |
	KEY_KP_PIPEPIPE    = 202,///< ||
	KEY_KP_COLON       = 203,
	KEY_KP_HASH        = 204, ///< #
	KEY_KP_SPACE       = 205,
	KEY_KP_AT          = 206, ///< @
	KEY_KP_BANG        = 207, ///< !
	KEY_KP_MEM_STORE   = 208, ///< Memory Store
	KEY_KP_MEM_RECALL  = 209,
	KEY_KP_MEM_CLEAR   = 210,
	KEY_KP_MEM_ADD     = 211,
	KEY_KP_MEM_SUB     = 212,
	KEY_KP_MEM_MUL     = 213,
	KEY_KP_MEM_DIV     = 214,
	KEY_KP_NEGATE  = 215, ///< +/-
	KEY_KP_CLEAR       = 216,
	KEY_KP_CLEAR_ENTRY = 217,
	KEY_KP_BINARY      = 218,
	KEY_KP_OCTAL       = 219,
	KEY_KP_DECIMAL     = 220,
	KEY_KP_HEX         = 221,

	KEY_RESERVED8 = 222,
	KEY_RESERVED9 = 223,

	KEY_LCTRL      = 224,
	KEY_LSHIFT     = 225,
	KEY_LALT       = 226,
	KEY_LGUI       = 227,
	KEY_RCTRL      = 228,
	KEY_RSHIFT     = 229,
	KEY_RALT       = 230,
	KEY_RGUI       = 231,
	// 232-65535 are reserved per USB but found the following
	KEY_MEDIA_PLAYPAUSE  = 232,
	KEY_MEDIA_STOPCD     = 233,
	KEY_MEDIA_PREV_SONG  = 234,
	KEY_MEDIA_NEXT_SONG  = 235,
	KEY_MEDIA_EJECT      = 236,
	KEY_MEDIA_VOL_UP     = 237,
	KEY_MEDIA_VOL_DOWN   = 238,
	KEY_MEDIA_MUTE       = 239,
	KEY_MEDIA_WWW        = 240,
	KEY_MEDIA_BACK       = 241,
	KEY_MEDIA_FORWARD    = 242,
	KEY_MEDIA_STOP       = 243,
	KEY_MEDIA_FIND       = 244,
	KEY_MEDIA_SCROLL_UP  = 245,
	KEY_MEDIA_SCROLL_DOWN= 246,
	KEY_MEDIA_EDIT       = 247,
	KEY_MEDIA_SLEEP      = 248,
	KEY_MEDIA_COFFEE     = 249,
	KEY_MEDIA_REFRESH    = 250,
	KEY_MEDIA_CALC       = 251
};

} // namespace platform



