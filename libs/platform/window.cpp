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

window::window( const std::shared_ptr<screen> &screen, const rect &p )
	: _rect( p ), _screen( screen )
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
		case event_type::DISPLAY_CHANGED:
		case event_type::APP_QUIT_REQUEST:
		case event_type::WINDOW_CLOSE_REQUEST:
		case event_type::WINDOW_DESTROYED:
			break;

		case event_type::WINDOW_SHOWN:
		case event_type::WINDOW_HIDDEN:
		case event_type::WINDOW_MINIMIZED:
		case event_type::WINDOW_MAXIMIZED:
		case event_type::WINDOW_RESTORED:
			_rect = query_geometry();
			break;

		case event_type::WINDOW_EXPOSED:
		case event_type::WINDOW_REGION_EXPOSED:
			break;

		case event_type::WINDOW_MOVED:
			_rect.set_x( e.window().x );
			_rect.set_y( e.window().y );
			break;
		case event_type::WINDOW_RESIZED:
			_rect.set_size( e.window().width, e.window().height );
			break;
		case event_type::WINDOW_MOVE_RESIZE:
			_rect.set( e.window().x, e.window().y, e.window().width, e.window().height );
			break;
		case event_type::MOUSE_ENTER:
		case event_type::MOUSE_LEAVE:
		case event_type::MOUSE_MOVE:
		case event_type::MOUSE_DOWN:
		case event_type::MOUSE_UP:
		case event_type::MOUSE_WHEEL:
		case event_type::DND_ENTER:
		case event_type::DND_LEAVE:
		case event_type::DND_MOVE:
		case event_type::DND_DROP_REQUEST:
		case event_type::KEYBOARD_DOWN:
		case event_type::KEYBOARD_UP:
		case event_type::KEYBOARD_REPEAT:
		case event_type::TEXT_ENTERED:
		case event_type::TABLET_DOWN:
		case event_type::TABLET_UP:
		case event_type::TABLET_MOVE:
		case event_type::TABLET_BUTTON:
		case event_type::HID_BUTTON_DOWN:
		case event_type::HID_BUTTON_UP:
		case event_type::HID_RELATIVE_WHEEL:
		case event_type::HID_SPINNER:
		case event_type::HID_DIAL_KNOB:
		case event_type::USER_EVENT:
		case event_type::NUM_EVENTS:
		default:
			break;
	}

	if ( event_handoff )
		return event_handoff( e );
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

void window::move( coord_type x, coord_type y )
{
	rect tmp = _rect;
	tmp.set_x( x );
	tmp.set_y( y );
	if ( update_geometry( tmp ) )
		_rect = tmp;
}

////////////////////////////////////////

void window::resize( coord_type w, coord_type h )
{
	rect tmp = _rect;
	tmp.set_size( w, h );
	if ( update_geometry( tmp ) )
		_rect = tmp;
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
	// TBD: do we need to do region compression ourselves?
	submit_delayed_expose( r );
}

////////////////////////////////////////

}

