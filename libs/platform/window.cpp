//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"
#include "event.h"
#include "system.h"
#include <base/contract.h>

namespace platform
{

////////////////////////////////////////

window::window( void )
{
}

////////////////////////////////////////

window::~window( void )
{
}
////////////////////////////////////////

bool
window::process_event( const event &e )
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
			expose_event();
			break;
		case event_type::WINDOW_MOVED:
			move_event( e.window().x, e.window().y );
			break;
		case event_type::WINDOW_RESIZED:
			resize_event( e.window().width, e.window().height );
			break;
		case event_type::WINDOW_MOVE_RESIZE:
			move_event( e.window().x, e.window().y );
			resize_event( e.window().width, e.window().height );
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
			if ( e.has_control_mod() && e.key().keys[0] == scancode::KEY_V )
			{
				auto sel = e.sys().query_selection( selection_type::CLIPBOARD );
				std::string val{ sel.first.begin(), sel.first.end() };
				std::cout << "Paste: '" << sel.second << "':\n" << val << std::endl;
			}
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
	return true;
}

////////////////////////////////////////

void
window::set_default_cursor( const std::shared_ptr<cursor> &c )
{
	_default_cursor = c;
	if ( _cursors.empty() )
		make_current( _default_cursor );
}

////////////////////////////////////////

void
window::push_cursor( const std::shared_ptr<cursor> &c )
{
	_cursors.push( c );
	make_current( c );
}

////////////////////////////////////////

void
window::pop_cursor( void )
{
	// let's be a bit more forgiving
	//precondition( ! _cursors.empty(), "un-matched pair of push / pop cursor stack" );
	if ( ! _cursors.empty() )
		_cursors.pop();

	if ( _cursors.empty() )
		make_current( _default_cursor );
	else
		make_current( _cursors.top() );
}

////////////////////////////////////////

}

