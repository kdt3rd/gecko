//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#include "simple_window.h"
#include <base/contract.h>
#include "event.h"

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

simple_window::simple_window( const std::shared_ptr<window> &win )
    : _win( win )
{
    precondition( _win, "expect valid window" );
    _win->event_handoff = [this] ( const event &e ) -> bool { return process_event( e ); };
}

////////////////////////////////////////

bool simple_window::process_event( const event &e )
{
	switch ( e.type() )
	{
		case event_type::WINDOW_SHOWN:
			if ( shown )
				shown();
			break;
		case event_type::WINDOW_HIDDEN:
			if ( hidden )
				hidden();
			break;
		case event_type::APP_QUIT_REQUEST:
		case event_type::WINDOW_CLOSE_REQUEST:
			if ( closed )
				return closed( false );
			break;
		case event_type::WINDOW_DESTROYED:
			if ( closed )
				closed( false );
			break;
		case event_type::WINDOW_MINIMIZED:
			if ( minimized )
				minimized();
			break;
		case event_type::WINDOW_MAXIMIZED:
			if ( maximized )
				maximized();
			break;
		case event_type::WINDOW_RESTORED:
			if ( restored )
				restored();
			break;
		case event_type::WINDOW_EXPOSED:
		case event_type::WINDOW_REGION_EXPOSED:
        {
            auto &ctxt = hw_context();
            auto guard = ctxt.begin_render();
			//expose_event( e.window().x, e.window().y, e.window().width, e.window().height );
            if ( exposed )
                exposed();
            ctxt.swap_buffers();
			break;
        }
		case event_type::WINDOW_MOVED:
            if ( moved )
                moved( e.window().x, e.window().y );
			break;
		case event_type::WINDOW_RESIZED:
            if ( resized )
                resized( e.window().width, e.window().height );
			break;
		case event_type::WINDOW_MOVE_RESIZE:
            if ( moved )
                moved( e.window().x, e.window().y );
            if ( resized )
                resized( e.window().width, e.window().height );
			break;
		case event_type::MOUSE_ENTER:
			if ( entered )
				entered();
			break;
		case event_type::MOUSE_LEAVE:
			if ( exited )
				exited();
			break;

		case event_type::MOUSE_MOVE:
			if ( mouse_moved )
				mouse_moved( e.source(), point( e.mouse().x, e.mouse().y ) );
			break;
		case event_type::MOUSE_DOWN:
			if ( mouse_pressed )
				mouse_pressed( e.source(), point( e.mouse().x, e.mouse().y ), e.mouse().button );
			break;
		case event_type::MOUSE_UP:
			if ( mouse_released )
				mouse_released( e.source(), point( e.mouse().x, e.mouse().y ), e.mouse().button );
			break;
		case event_type::MOUSE_WHEEL:
			if ( mouse_wheel )
				mouse_wheel( e.source(), e.hid().position );
			break;

		case event_type::DND_ENTER:
		case event_type::DND_LEAVE:
		case event_type::DND_MOVE:
		case event_type::DND_DROP_REQUEST:
			break;

		case event_type::KEYBOARD_DOWN:
			if ( key_pressed )
				key_pressed( e.source(), e.key().keys[0] );
			break;
		case event_type::KEYBOARD_UP:
			if ( key_released )
				key_released( e.source(), e.key().keys[0] );
			break;

		case event_type::KEYBOARD_REPEAT:
			if ( key_pressed )
				key_pressed( e.source(), e.key().keys[0] );
			break;

		case event_type::TEXT_ENTERED:
			if ( text_entered )
				text_entered( e.source(), e.text().text );
			break;

		case event_type::TABLET_DOWN:
		case event_type::TABLET_UP:
		case event_type::TABLET_MOVE:
		case event_type::TABLET_BUTTON:
			break;

		case event_type::HID_BUTTON_DOWN:
		case event_type::HID_BUTTON_UP:
		case event_type::HID_RELATIVE_WHEEL:
		case event_type::HID_SPINNER:
		case event_type::HID_DIAL_KNOB:
			break;
		case event_type::USER_EVENT:
			break;
		case event_type::NUM_EVENTS:
		default:
			break;
	}

	if ( event_handoff )
		return event_handoff( e );
    return true;
}

////////////////////////////////////////

} // platform



