// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <platform/event.h>
#include <base/contract.h>
#include "types.h"

namespace gui
{

class window;

class event
{
public:
    event( window &w, const platform::event &pevt ) : _w( w ), _pe( pevt ) {}

    inline platform::event_type type( void ) const { return _pe.type(); }

	inline uint8_t modifiers( void ) const { return _pe.modifiers(); }
	inline bool has_only_mod( uint8_t mask ) const { return _pe.has_only_mod( mask ); }
	inline bool has_control_mod( void ) const { return _pe.has_control_mod(); }
	inline bool has_shift_mod( void ) const { return _pe.has_shift_mod(); }
	inline bool has_alt_mod( void ) const { return _pe.has_alt_mod(); }
	inline bool has_meta_mod( void ) const { return _pe.has_meta_mod(); }

    window &win( void ) const { return _w; }

    coord from_native_horiz( const platform::coord_type &c ) const;
    coord from_native_vert( const platform::coord_type &c ) const;
    point from_native( const platform::point &p ) const;
    point from_native( platform::coord_type x, platform::coord_type y ) const;
    size from_native( const platform::size &s ) const;
    rect from_native( const platform::rect &r ) const;

    const platform::event &raw_event( void ) const { return _pe; }

    const platform::event::window_info &raw_window( void ) const { return _pe.window(); }
	const platform::event::mouse_info &raw_mouse( void ) const { return _pe.mouse(); }
	const platform::event::key_info &raw_key( void ) const { return _pe.key(); }
	const platform::event::text_info &raw_text( void ) const { return _pe.text(); }
	const platform::event::tablet_info &raw_tablet( void ) const { return _pe.tablet(); }
	const platform::event::hid_info &raw_hid( void ) const { return _pe.hid(); }
	const platform::event::user_info &raw_user( void ) const { return _pe.user(); }

	platform::system &sys( void ) const { return _pe.sys(); }
	platform::event_source &source( void ) const { return _pe.source(); }

private:
    window &_w;
    const platform::event &_pe;
};

} // namespace gui
