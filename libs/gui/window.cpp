// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "window.h"

#include "application.h"
#include "widget.h"

#include <base/contract.h>
#include <base/scope_guard.h>
#include <iostream>
#include <platform/event.h>
#include <platform/window.h>

namespace gui
{
////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &win ) : _window( win )
{
    precondition( bool( _window ), "null window" );
    _window->event_handoff = [this]( const platform::event &e ) -> bool {
        return process_event( event( *this, e ) );
    };
}

////////////////////////////////////////

window::~window( void ) {}

////////////////////////////////////////

void window::set_title( const std::string &t ) { _window->set_title( t ); }

////////////////////////////////////////

void window::set_default_cursor( const std::shared_ptr<platform::cursor> &c )
{
    _window->set_default_cursor( c );
}

////////////////////////////////////////

void window::push_cursor( const std::shared_ptr<platform::cursor> &c )
{
    _window->push_cursor( c );
}

////////////////////////////////////////

void window::pop_cursor( void ) { _window->pop_cursor(); }

////////////////////////////////////////

void window::show( void ) { _window->show(); }

////////////////////////////////////////

void window::hide( void ) { _window->hide(); }

////////////////////////////////////////

void window::move( coord x, coord y )
{
    _window->move( to_native( point( x, y ) ) );
}

////////////////////////////////////////

void window::resize( coord w, coord h )
{
    _window->resize( to_native( size( w, h ) ) );
}

////////////////////////////////////////

void window::set_widget( const std::shared_ptr<widget> &w )
{
    in_context( [&, this] {
        _widget = w;
        if ( _widget )
        {
            _widget->build( *this );
            _widget->layout_target()->compute_bounds();
            _widget->layout_target()->set_horizontal(
                0.0, _window->width() - 1.0 );
            _widget->layout_target()->set_vertical(
                0.0, _window->height() - 1.0 );
        }
    } );
}

////////////////////////////////////////

coord window::width( void ) const
{
    return from_native_horiz( _window->width() );
}

////////////////////////////////////////

coord window::height( void ) const
{
    return from_native_vert( _window->height() );
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
    _internal_paint = true;
    _window->invalidate( to_native( r.round() ) );
}

////////////////////////////////////////

coord window::from_native_horiz( platform::coord_type c ) const
{
    return _window->query_screen()->to_physical_horiz( c );
}

////////////////////////////////////////

coord window::from_native_vert( platform::coord_type c ) const
{
    return _window->query_screen()->to_physical_vert( c );
}

////////////////////////////////////////

point window::from_native( const platform::point &p ) const
{
    return _window->query_screen()->to_physical( p );
}

////////////////////////////////////////

point window::from_native(
    platform::coord_type x, platform::coord_type y ) const
{
    return _window->query_screen()->to_physical( platform::point( x, y ) );
}

////////////////////////////////////////

size window::from_native( const platform::size &s ) const
{
    return _window->query_screen()->to_physical( s );
}

////////////////////////////////////////

rect window::from_native( const platform::rect &r ) const
{
    return _window->query_screen()->to_physical( r );
}

////////////////////////////////////////

platform::coord_type window::to_native_horiz( const coord &c ) const
{
    return _window->query_screen()->to_native_horiz( c );
}

////////////////////////////////////////

platform::coord_type window::to_native_vert( const coord &c ) const
{
    return _window->query_screen()->to_native_vert( c );
}

////////////////////////////////////////

platform::point window::to_native( const point &p ) const
{
    return _window->query_screen()->to_native( p );
}

////////////////////////////////////////

platform::size window::to_native( const size &s ) const
{
    return _window->query_screen()->to_native( s );
}

////////////////////////////////////////

platform::rect window::to_native( const rect &r ) const
{
    return _window->query_screen()->to_native( r );
}

////////////////////////////////////////

platform::context &window::hw_context( void ) { return _window->hw_context(); }

////////////////////////////////////////

void window::grab_source( const event &e, std::shared_ptr<widget> w )
{
    e.source().grab( _window );
    // TODO: not general...
    _mouse_grab = w;
}

////////////////////////////////////////

void window::release_source( const event &e )
{
    _mouse_grab.reset();
    e.source().ungrab();
}

////////////////////////////////////////

void window::set_focus( std::shared_ptr<widget> w )
{
    _key_focus = std::move( w );
}

////////////////////////////////////////

platform::context::render_guard window::bind( void )
{
    return _window->hw_context().begin_render();
}

////////////////////////////////////////

bool window::process_event( const event &e )
{
    // we're doing this in the sub functions right now...
    push_context();
    on_scope_exit { pop_context(); };

    using namespace platform;
    switch ( e.type() )
    {
        case event_type::DISPLAY_CHANGED:
            // anything else to do?
            application::current()->update_display( this );
            if ( _widget )
                in_context( [&, this] { _widget->monitor_changed( *this ); } );
            break;

        case event_type::APP_QUIT_REQUEST:
            return application::current()->process_quit_request();

        case event_type::WINDOW_CLOSE_REQUEST: return close_request( e );

        case event_type::WINDOW_DESTROYED:
            // to make sure the window isn't used...
            _window.reset();
            application::current()->window_destroyed( this );
            return true;

        case event_type::WINDOW_SHOWN: break;

        case event_type::WINDOW_HIDDEN:
            break;

            // do we care about the subtle difference implied by these?
            // for now, just use the shown / hidden above...
        case event_type::WINDOW_MINIMIZED:
        case event_type::WINDOW_RESTORED: break;

        case event_type::WINDOW_MAXIMIZED: break;

        case event_type::WINDOW_EXPOSED: paint( rect() ); break;
        case event_type::WINDOW_REGION_EXPOSED:
            paint( from_native( platform::rect(
                e.raw_window().x,
                e.raw_window().y,
                e.raw_window().width,
                e.raw_window().height ) ) );
            break;

        case event_type::WINDOW_MOVED: break;
        case event_type::WINDOW_RESIZED:
            resized( from_native( platform::size(
                e.raw_window().width, e.raw_window().height ) ) );
            break;
        case event_type::WINDOW_MOVE_RESIZE:
            resized( from_native( platform::size(
                e.raw_window().width, e.raw_window().height ) ) );
            break;
        case event_type::MOUSE_ENTER: break;
        case event_type::MOUSE_LEAVE: break;

        case event_type::MOUSE_MOVE:
            if ( _mouse_grab )
                _mouse_grab->mouse_move( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_mouse().x, e.raw_mouse().y ) );
                if ( w )
                    w->mouse_move( e );
            }
            break;
        case event_type::MOUSE_DOWN:
            if ( _mouse_grab )
                _mouse_grab->mouse_press( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_mouse().x, e.raw_mouse().y ) );
                if ( w )
                    w->mouse_press( e );
            }
            break;
        case event_type::MOUSE_UP:
            if ( _mouse_grab )
                _mouse_grab->mouse_release( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_mouse().x, e.raw_mouse().y ) );
                if ( w )
                    w->mouse_release( e );
            }
            break;
        case event_type::MOUSE_WHEEL:
            if ( _mouse_grab )
                _mouse_grab->mouse_wheel( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_hid().x, e.raw_hid().y ) );
                if ( w )
                    w->mouse_wheel( e );
            }
            //			mouse_wheel( e.hid().position );
            break;

        case event_type::DND_ENTER:
        case event_type::DND_LEAVE:
        case event_type::DND_MOVE:
        case event_type::DND_DROP_REQUEST: break;

        case event_type::KEYBOARD_DOWN: key_down( e ); break;
        case event_type::KEYBOARD_REPEAT: key_repeat( e ); break;
        case event_type::KEYBOARD_UP:
            if ( _key_focus )
                _key_focus->key_release( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_key().x, e.raw_key().y ) );
                if ( w )
                    w->key_release( e );
            }
            break;
        case event_type::TEXT_ENTERED:
            if ( _key_focus )
                _key_focus->text_input( e );
            else if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_key().x, e.raw_key().y ) );
                if ( w )
                    w->text_input( e );
            }
            break;

        case event_type::TABLET_DOWN:
        case event_type::TABLET_UP:
        case event_type::TABLET_MOVE:
        case event_type::TABLET_BUTTON:
            if ( _widget )
            {
                auto w = _widget->find_widget_under(
                    from_native( e.raw_tablet().x, e.raw_tablet().y ) );
                if ( w )
                    w->tablet_event( e );
            }
            break;

        case event_type::HID_BUTTON_DOWN:
        case event_type::HID_BUTTON_UP:
        case event_type::HID_RELATIVE_WHEEL:
        case event_type::HID_SPINNER:
        case event_type::HID_DIAL_KNOB:
            // TODO: focus???
            if ( _widget )
                _widget->hid_event( e );
            break;

        case event_type::USER_EVENT:
            // TODO: focus???
            if ( _widget )
                in_context( [&, this] { _widget->user_event( e ); } );
            break;

        case event_type::NUM_EVENTS:
        default: break;
    }

    return true;
}

////////////////////////////////////////

bool window::close_request( const event &e ) { return true; }

////////////////////////////////////////

void window::paint( const rect &r )
{
    // in case we were destroyed but still are processing paint requests...
    if ( !_window )
        return;

    platform::context &hwctxt = _window->hw_context();
    auto               guard  = hwctxt.begin_render();

    auto clipguard =
        hwctxt.push_clip( r.empty() ? _window->bounds() : to_native( r ) );

    gl::api &ogl = hwctxt.api();
    ogl.reset();
    ogl.enable( gl::capability::MULTISAMPLE );
    ogl.enable( gl::capability::BLEND );
    ogl.blend_func(
        gl::blend_style::SRC_ALPHA, gl::blend_style::ONE_MINUS_SRC_ALPHA );

    ogl.clear_color( _style.background_color() );
    ogl.set_projection(
        gl::matrix4::ortho( 0, width().count(), 0, height().count() ) );
    ogl.clear();

    //	std::cout << "model matrix: " << ogl.model_matrix() << std::endl;
    //	std::cout << "view matrix: " << ogl.view_matrix() << std::endl;
    //	std::cout << "projection matrix: " << ogl.projection_matrix() << std::endl;
    //	std::cout << "current matrix: " << ogl.current_matrix() << std::endl;

    if ( _widget )
    {
        in_context( [&, this] {
            if ( _internal_paint )
            {
                _widget->layout_target()->compute_bounds();
                _widget->set_size( width(), height() );
                _widget->layout_target()->set_size( width(), height() );
                _widget->layout_target()->compute_layout();
                if ( _widget->update_layout( 250.0 ) )
                    invalidate( *_widget );
                else
                    _internal_paint = false;
            }
            _widget->paint( *this );
        } );
    }

    hwctxt.swap_buffers();
}

////////////////////////////////////////

void window::resized( const size &s )
{
    if ( _widget )
    {
        in_context( [&, this] {
            rect r = { point(), s };

            hw_context().viewport( 0, 0, _window->width(), _window->height() );

            _widget->set( r );
            _widget->layout_target()->set( r );
            _widget->layout_target()->compute_layout();
            _widget->update_layout( 0.0 );
            // the ui will send a paint request, we don't need to double that up...
            //			invalidate( *_widget );
            _internal_paint = false;
        } );
    }
}

////////////////////////////////////////

void window::key_down( const event &e )
{
    // TODO: don't allow multiple scancode combinations?
    if ( e.raw_key().keys[1] != platform::scancode::KEY_NO_EVENT )
        return;

#if defined( __APPLE__ )
    if ( !e.has_only_mod(
             platform::modifier::LEFT_META | platform::modifier::RIGHT_META ) )
        return;
#else
    if ( !e.has_only_mod(
             platform::modifier::LEFT_CTRL | platform::modifier::RIGHT_CTRL ) )
        return;
#endif

    platform::scancode sc = e.raw_key().keys[0];

    if ( application::current()->dispatch_global_hotkey( e ) )
        return;

    auto i = _hotkeys.find( sc );
    if ( i != _hotkeys.end() )
    {
        ( i->second )( from_native( e.raw_key().x, e.raw_key().y ) );
        return;
    }

    if ( _key_focus )
        _key_focus->key_press( e );
    else if ( _widget )
    {
        auto w = _widget->find_widget_under(
            from_native( e.raw_key().x, e.raw_key().y ) );
        if ( w )
            w->key_press( e );
    }
}

////////////////////////////////////////

void window::key_repeat( const event &e )
{
    // TODO: do we need to differentiate this?
    key_down( e );
}

////////////////////////////////////////

} // namespace gui
