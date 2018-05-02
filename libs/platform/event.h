//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <string>

#include "scancode.h"
#include "types.h"

////////////////////////////////////////

namespace platform
{

class system;
class event_source;

enum event_type : uint8_t
{
	DISPLAY_CHANGED,
	APP_QUIT_REQUEST,

	WINDOW_SHOWN,
	WINDOW_HIDDEN,
	WINDOW_CLOSE_REQUEST,
	WINDOW_DESTROYED,
	WINDOW_MINIMIZED,
	WINDOW_MAXIMIZED,
	WINDOW_RESTORED,

	WINDOW_EXPOSED,
	WINDOW_REGION_EXPOSED,
	WINDOW_MOVED,
	WINDOW_RESIZED,
	WINDOW_MOVE_RESIZE,

	// technically they are just buttons or other HID events, but
	// so common, let's treat them specially
	MOUSE_ENTER,
	MOUSE_LEAVE,
	MOUSE_MOVE,
	MOUSE_DOWN,
	MOUSE_UP,
	MOUSE_WHEEL,

	DND_ENTER,
	DND_LEAVE,
	DND_MOVE,
	DND_DROP_REQUEST,

	KEYBOARD_DOWN,
	KEYBOARD_UP,
	KEYBOARD_REPEAT,

	TEXT_ENTERED,

	TABLET_DOWN,
	TABLET_UP,
	TABLET_MOVE, // TODO: needed?
	TABLET_BUTTON, // should be used for pen side button or buttons on tablet, is there a standard?

	HID_BUTTON_DOWN,
	HID_BUTTON_UP,
	HID_RELATIVE_WHEEL, ///< shuttle advance / reverse but indicates a magnitude
	HID_SPINNER, ///< rotating wheel that spins infinitely (a mouse
				 ///< wheel but not always vertically oriented)
	HID_DIAL_KNOB, ///< like a volume or other dial that has a min and max extent

	USER_EVENT, ///< a generic user generated event

	NUM_EVENTS
};

///
/// @brief Class event provides a basic container to carry information about events
///
/// This is transformed by the dispatcher from the @event_source to
/// deliver to the @sa event_target
///
/// NB: all the events currently have different data layouts, but are
/// a max of 16 bytes of data then + 1 to hold the type.
///
/// TODO: is there other data we could pass? Most implementations will
/// pad this to at least 20 bytes... and at that point, might as well
/// go to 32 to get even L2 alignment? 15 extra bytes gives 3 int32_t
/// and 3 bytes...
///
class event
{
public:
	struct window_info
	{
		coord_type x, y;
		coord_type width, height;
	};
	struct mouse_info
	{
		coord_type x, y;
		coord_type button;
	};
	struct key_info
	{
		coord_type x, y; // pointer position
		scancode keys[6]; // per usb spec, if > 6 keys down, all report overflow...
	};
	struct text_info
	{
		coord_type x, y; // pointer position
		char32_t text;
	};
	struct tablet_info
	{
		coord_type x, y;
		int16_t button; // TODO: do we need to differentiate pad ring / buttons from tool buttons?
		int16_t pressure; // negative indicates distance
		int16_t angle;
	};
	struct hid_info
	{
		// TODO: is negative useful for element or should this be
		// uint32_t, w/ uint32_t(-1) meaning something special (all,
		// none, unknown)?
		int32_t element; // which one of the buttons / dials / thingies
		int32_t position; // what's the detent / delta
	};
	struct user_info
	{
		uint32_t id;
		void *data;
	};

	inline event_type type( void ) const { return _type; }

	inline bool has_control_mod( void ) const { return ( _modifiers & ( modifier::LEFT_CTRL | modifier::RIGHT_CTRL ) ) != 0; }
	inline bool has_shift_mod( void ) const { return ( _modifiers & ( modifier::LEFT_SHIFT | modifier::RIGHT_SHIFT ) ) != 0; }
	inline bool has_alt_mod( void ) const { return ( _modifiers & ( modifier::LEFT_ALT | modifier::RIGHT_ALT ) ) != 0; }
	inline bool has_meta_mod( void ) const { return ( _modifiers & ( modifier::LEFT_META | modifier::RIGHT_META ) ) != 0; }

	const window_info &window( void ) const { return _data.window; }
	const mouse_info &mouse( void ) const { return _data.mouse; }
	const key_info &key( void ) const { return _data.key; }
	const text_info &text( void ) const { return _data.text; }
	const tablet_info &tablet( void ) const { return _data.tablet; }
	const hid_info &hid( void ) const { return _data.hid; }
	const user_info &user( void ) const { return _data.user; }

	system &sys( void ) const { return *_system; }
	event_source &source( void ) const { return *_source; }

	static inline event window( system *sys, event_source *src, event_type et, coord_type x, coord_type y, coord_type w, coord_type h )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.window = { x, y, w, h };
		r._modifiers = 0;
		return r;
	}

	static inline event key( system *sys, event_source *src, event_type et, coord_type x, coord_type y, scancode kc, uint8_t mods )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.key.x = x;
		r._data.key.y = y;
		r._data.key.keys[0] = kc;
		r._data.key.keys[1] = scancode::KEY_NO_EVENT;
		r._data.key.keys[2] = scancode::KEY_NO_EVENT;
		r._data.key.keys[3] = scancode::KEY_NO_EVENT;
		r._data.key.keys[4] = scancode::KEY_NO_EVENT;
		r._data.key.keys[5] = scancode::KEY_NO_EVENT;
		r._modifiers = mods;
		return r;
	}

	static inline event key( system *sys, event_source *src, event_type et, coord_type x, coord_type y, scancode kc[6], uint8_t mods )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.key.x = x;
		r._data.key.y = y;
		for ( int i = 0; i < 6; ++i )
			r._data.key.keys[i] = kc[i];
		r._modifiers = mods;
		return r;
	}

	static inline event mouse( system *sys, event_source *src, event_type et, coord_type x, coord_type y, coord_type b, uint8_t mods )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.mouse.x = x;
		r._data.mouse.y = y;
		r._data.mouse.button = b;
		r._modifiers = mods;
		return r;
	}

	static inline event text( system *sys, event_source *src, event_type et, coord_type x, coord_type y, char32_t c, uint8_t mods )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.text.x = x;
		r._data.text.y = y;
		r._data.text.text = c;
		r._modifiers = mods;
		return r;
	}

	static inline event hid( system *sys, event_source *src, event_type et, int32_t elt, int32_t pos, uint8_t mods )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = et;
		r._data.hid.element = elt;
		r._data.hid.position = pos;
		r._modifiers = mods;
		return r;
	}

	static inline event user( system *sys, event_source *src, uint32_t id, void *data )
	{
		event r;
		r._system = sys;
		r._source = src;
		r._type = USER_EVENT;
		r._data.user.id = id;
		r._data.user.data = data;
		return r;
	}

	static uint32_t register_user_event( const std::string &name );

private:
	system *_system = nullptr;
	event_source *_source = nullptr;

	union
	{
		window_info window;
		mouse_info mouse;
		key_info key;
		text_info text;
		tablet_info tablet;
		hid_info hid;
		user_info user;
	} _data;

	event_type _type;
	uint8_t _modifiers = 0;
};

} // namespace platform



