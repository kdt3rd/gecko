// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "window.h"

#include "event.h"
#include "system.h"

#include <base/contract.h>

namespace platform
{
////////////////////////////////////////

window::window( window_type wt, const std::shared_ptr<screen> &screen, const rect &p )
    : _win_type( wt ), _rect( p ), _screen( screen )
{}

////////////////////////////////////////

window::~window( void ) {}
////////////////////////////////////////

bool window::process_event( const event &e )
{
    switch ( e.type() )
    {
        case event_type::DISPLAY_CHANGED:
        case event_type::APP_QUIT_REQUEST:
        case event_type::WINDOW_CLOSE_REQUEST:
        case event_type::WINDOW_DESTROYED: break;

        case event_type::WINDOW_SHOWN:
        case event_type::WINDOW_HIDDEN:
        case event_type::WINDOW_MINIMIZED:
        case event_type::WINDOW_MAXIMIZED:
        case event_type::WINDOW_RESTORED:
            _rect = query_geometry();
            break;

            // any difference?
        case event_type::WINDOW_EXPOSED:
        case event_type::WINDOW_REGION_EXPOSED:
        {
            rect tmp{ e.window().x, e.window().y, e.window().width, e.window().height };
            tmp.include( _invalid_rgn );

            _accumulate_expose = false;
            _invalid_rgn       = rect();

            event incE = event::window(
                &e.source(), e.type(), tmp.x(), tmp.y(), tmp.width(), tmp.height() );

            // shortcut since we've created a custom event to incorporate the region
            if ( event_handoff )
                return event_handoff( incE );
            break;
        }

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
        case event_type::NUM_EVENTS: break;
    }

    if ( event_handoff )
        return event_handoff( e );
    return true;
}

////////////////////////////////////////

void window::set_default_cursor( const std::shared_ptr<cursor> &c )
{
    _default_cursor = c;
    if ( _cursors.empty() )
        make_current( _default_cursor );
}

////////////////////////////////////////

void window::push_cursor( const std::shared_ptr<cursor> &c )
{
    _cursors.push( c );
    make_current( c );
}

////////////////////////////////////////

void window::pop_cursor( void )
{
    // let's be a bit more forgiving
    //precondition( ! _cursors.empty(), "un-matched pair of push / pop cursor stack" );
    if ( !_cursors.empty() )
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

void window::move( const phys_point &p )
{
    phys_rect tmp = phys_bounds();
    tmp.set_position( p );
    rect nat = _screen->to_native( tmp );
    if ( update_geometry( nat ) )
        _rect = nat;
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

void window::resize( const phys_size &s )
{
    phys_rect tmp = phys_bounds();
    tmp.set_extent( s );
    rect nat = _screen->to_native( tmp );
    if ( update_geometry( nat ) )
        _rect = nat;
}

////////////////////////////////////////

void window::set_minimum_size( coord_type w, coord_type h )
{
    apply_minimum_size( w, h );
}

////////////////////////////////////////

void window::set_minimum_size( const phys_size &s )
{
    size nat = _screen->to_native( s );
    apply_minimum_size( nat.w(), nat.h() );
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
    _invalid_rgn.include( r );

    // invalidation compression...
    if ( _accumulate_expose )
        return;

    submit_delayed_expose( r );
}

////////////////////////////////////////

phys_rect window::phys_bounds( void ) const { return _screen->to_physical( _rect ); }

////////////////////////////////////////

} // namespace platform
