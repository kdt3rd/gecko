// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "scancode.h"
#include "system.h"
#include "types.h"

#include <cstdint>
#include <string>

////////////////////////////////////////

namespace platform
{
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
    HID_SPINNER,        ///< rotating wheel that spins infinitely (a mouse
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
        int        button;
    };
    struct key_info
    {
        coord_type x, y; // pointer position
        scancode
            keys[6]; // per usb spec, if > 6 keys down, all report overflow...
    };
    struct text_info
    {
        coord_type x, y; // pointer position
        char32_t   text;
    };
    struct tablet_info
    {
        coord_type x, y;
        int16_t
                button; // TODO: do we need to differentiate pad ring / buttons from tool buttons?
        int16_t pressure; // negative indicates distance
        int16_t angle;
    };
    struct hid_info
    {
        // TODO: is negative useful for element or should this be
        // uint16_t, w/ uint16_t(-1) meaning something special (all,
        // none, unknown)?
        coord_type x, y;     // pointer position
        int16_t    element;  // which one of the buttons / dials / thingies
        int16_t    position; // what's the detent / delta
    };
    struct user_info
    {
        uint32_t id;
        void *   data;
    };

    inline event_type type( void ) const { return _type; }

    inline uint8_t modifiers( void ) const { return _modifiers; }
    inline bool    has_only_mod( uint8_t mask ) const
    {
        return ( _modifiers & mask ) == _modifiers;
    }
    inline bool has_control_mod( void ) const
    {
        return ( _modifiers &
                 ( modifier::LEFT_CTRL | modifier::RIGHT_CTRL ) ) != 0;
    }
    inline bool has_shift_mod( void ) const
    {
        return ( _modifiers &
                 ( modifier::LEFT_SHIFT | modifier::RIGHT_SHIFT ) ) != 0;
    }
    inline bool has_alt_mod( void ) const
    {
        return ( _modifiers & ( modifier::LEFT_ALT | modifier::RIGHT_ALT ) ) !=
               0;
    }
    inline bool has_meta_mod( void ) const
    {
        return ( _modifiers &
                 ( modifier::LEFT_META | modifier::RIGHT_META ) ) != 0;
    }

    const window_info &window( void ) const { return _data.window; }
    const mouse_info & mouse( void ) const { return _data.mouse; }
    const key_info &   key( void ) const { return _data.key; }
    const text_info &  text( void ) const { return _data.text; }
    const tablet_info &tablet( void ) const { return _data.tablet; }
    const hid_info &   hid( void ) const { return _data.hid; }
    const user_info &  user( void ) const { return _data.user; }

    system &      sys( void ) const { return *( _source->get_system() ); }
    event_source &source( void ) const { return *_source; }

    static inline event window(
        event_source *src,
        event_type    et,
        coord_type    x,
        coord_type    y,
        coord_type    w,
        coord_type    h )
    {
        event r;
        r._source      = src;
        r._type        = et;
        r._data.window = { x, y, w, h };
        r._modifiers   = 0;
        return r;
    }

    static inline event
    key( event_source *src,
         event_type    et,
         coord_type    x,
         coord_type    y,
         scancode      kc,
         uint8_t       mods )
    {
        event r;
        r._source           = src;
        r._type             = et;
        r._data.key.x       = x;
        r._data.key.y       = y;
        r._data.key.keys[0] = kc;
        r._data.key.keys[1] = scancode::KEY_NO_EVENT;
        r._data.key.keys[2] = scancode::KEY_NO_EVENT;
        r._data.key.keys[3] = scancode::KEY_NO_EVENT;
        r._data.key.keys[4] = scancode::KEY_NO_EVENT;
        r._data.key.keys[5] = scancode::KEY_NO_EVENT;
        r._modifiers        = mods;
        return r;
    }

    static inline event
    key( event_source *src,
         event_type    et,
         coord_type    x,
         coord_type    y,
         scancode      kc[6],
         uint8_t       mods )
    {
        event r;
        r._source     = src;
        r._type       = et;
        r._data.key.x = x;
        r._data.key.y = y;
        for ( int i = 0; i < 6; ++i )
            r._data.key.keys[i] = kc[i];
        r._modifiers = mods;
        return r;
    }

    static inline event mouse(
        event_source *src,
        event_type    et,
        coord_type    x,
        coord_type    y,
        int           b,
        uint8_t       mods )
    {
        event r;
        r._source            = src;
        r._type              = et;
        r._data.mouse.x      = x;
        r._data.mouse.y      = y;
        r._data.mouse.button = b;
        r._modifiers         = mods;
        return r;
    }

    static inline event text(
        event_source *src,
        event_type    et,
        coord_type    x,
        coord_type    y,
        char32_t      c,
        uint8_t       mods )
    {
        event r;
        r._source         = src;
        r._type           = et;
        r._data.text.x    = x;
        r._data.text.y    = y;
        r._data.text.text = c;
        r._modifiers      = mods;
        return r;
    }

    static inline event
    hid( event_source *src,
         event_type    et,
         coord_type    x,
         coord_type    y,
         int16_t       elt,
         int16_t       pos,
         uint8_t       mods )
    {
        event r;
        r._source            = src;
        r._type              = et;
        r._data.hid.x        = x;
        r._data.hid.y        = y;
        r._data.hid.element  = elt;
        r._data.hid.position = pos;
        r._modifiers         = mods;
        return r;
    }

    static inline event user( event_source *src, uint32_t id, void *data )
    {
        event r;
        r._source         = src;
        r._type           = USER_EVENT;
        r._data.user.id   = id;
        r._data.user.data = data;
        return r;
    }

    static uint32_t register_user_event( const std::string &name );

private:
    event_source *_source = nullptr;

    union
    {
        window_info window;
        mouse_info  mouse;
        key_info    key;
        text_info   text;
        tablet_info tablet;
        hid_info    hid;
        user_info   user;
    } _data;

    event_type _type;
    uint8_t    _modifiers = 0;
};

} // namespace platform
