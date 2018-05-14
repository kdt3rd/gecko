//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "window.h"
#include "widget.h"
#include "application.h"
#include <platform/window.h>
#include <platform/event.h>
#include <base/contract.h>
#include <base/scope_guard.h>

namespace gui
{

////////////////////////////////////////

window::window( const std::shared_ptr<platform::window> &win )
	: _window( win )
{
	precondition( bool(_window), "null window" );
	_window->event_handoff = [this] ( const event &e ) -> bool { return process_event( e ); };
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	_window->set_title( t );
}

////////////////////////////////////////

void
window::set_default_cursor( const std::shared_ptr<platform::cursor> &c )
{
	_window->set_default_cursor( c );
}

////////////////////////////////////////

void
window::push_cursor( const std::shared_ptr<platform::cursor> &c )
{
	_window->push_cursor( c );
}

////////////////////////////////////////

void
window::pop_cursor( void )
{
	_window->pop_cursor();
}

////////////////////////////////////////

void window::show( void )
{
	_window->show();
}

////////////////////////////////////////

void window::hide( void )
{
	_window->hide();
}

////////////////////////////////////////

void window::move( coord x, coord y )
{
	_window->move( x, y );
}

////////////////////////////////////////

void window::resize( coord w, coord h )
{
	_window->resize( w, h );
}

////////////////////////////////////////

void window::set_widget( const std::shared_ptr<widget> &w )
{
	in_context( [&,this]
	{
		_widget = w;
		_widget->build( *this );
		_widget->layout_target()->compute_bounds();
		_widget->layout_target()->set_horizontal( 0.0, _window->width() - 1.0 );
		_widget->layout_target()->set_vertical( 0.0, _window->height() - 1.0 );
	} );
}

////////////////////////////////////////

coord window::width( void ) const
{
	return _window->width();
}

////////////////////////////////////////

coord window::height( void ) const
{
	return _window->height();
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
	_window->invalidate( platform::rect( r.round() ) );
}

////////////////////////////////////////

platform::context &window::hw_context( void )
{
	return _window->hw_context();
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
//	push_context();
//	on_scope_exit { pop_context(); };

	using namespace platform;
	switch ( e.type() )
	{
		case event_type::DISPLAY_CHANGED:
			break;

		case event_type::APP_QUIT_REQUEST:
			return application::current()->process_quit_request();

		case event_type::WINDOW_CLOSE_REQUEST:
			return close_request( e );

		case event_type::WINDOW_DESTROYED:
			// to make sure the window isn't used...
			_window.reset();
			application::current()->window_destroyed( this );
			return true;

		case event_type::WINDOW_SHOWN:
			break;

		case event_type::WINDOW_HIDDEN:
			break;

			// do we care about the subtle difference implied by these?
			// for now, just use the shown / hidden above...
		case event_type::WINDOW_MINIMIZED:
		case event_type::WINDOW_RESTORED:
			break;

		case event_type::WINDOW_MAXIMIZED:
			break;

		case event_type::WINDOW_EXPOSED:
			paint( 0, 0, 0, 0 );
			break;
		case event_type::WINDOW_REGION_EXPOSED:
			paint( e.window().x, e.window().y, e.window().width, e.window().height );
			break;

		case event_type::WINDOW_MOVED:
			break;
		case event_type::WINDOW_RESIZED:
			resized( e.window().width, e.window().height );
			break;
		case event_type::WINDOW_MOVE_RESIZE:
			resized( e.window().width, e.window().height );
			break;
		case event_type::MOUSE_ENTER:
			break;
		case event_type::MOUSE_LEAVE:
			break;

		case event_type::MOUSE_MOVE:
			mouse_moved( point( e.mouse().x, e.mouse().y ) );
			break;
		case event_type::MOUSE_DOWN:
			mouse_press( point( e.mouse().x, e.mouse().y ), e.mouse().button );
			break;
		case event_type::MOUSE_UP:
			mouse_release( point( e.mouse().x, e.mouse().y ), e.mouse().button );
			break;
		case event_type::MOUSE_WHEEL:
			mouse_wheel( e.hid().position );
			break;

		case event_type::DND_ENTER:
		case event_type::DND_LEAVE:
		case event_type::DND_MOVE:
		case event_type::DND_DROP_REQUEST:
			break;

		case event_type::KEYBOARD_DOWN:
			key_pressed( e.key().keys[0] );
			break;
		case event_type::KEYBOARD_UP:
			key_released( e.key().keys[0] );
			break;
		case event_type::KEYBOARD_REPEAT:
			key_pressed( e.key().keys[0] );
			break;
		case event_type::TEXT_ENTERED:
			text_entered( e.text().text );
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

bool window::close_request( const event &e )
{
	return true;
}

////////////////////////////////////////

void window::paint( coord dx, coord dy, coord dw, coord dh )
{
	// in case we were destroyed but still are processing paint requests...
	if ( ! _window )
		return;

	coord w = _window->width();
	coord h = _window->height();

	platform::context &hwctxt = _window->hw_context();
	auto guard = hwctxt.begin_render();

	gl::api &ogl = hwctxt.api();
	ogl.reset();
	ogl.viewport( 0, 0, w, h );

	bool didpushsc = false;
	if ( dw != 0 || dh != 0 )
	{
		ogl.push_scissor( dx, dy, dw, dh );
		didpushsc = true;
	}
	on_scope_exit { if ( didpushsc ) ogl.pop_scissor(); };

	ogl.enable( gl::capability::MULTISAMPLE );
	ogl.enable( gl::capability::BLEND );
    ogl.blend_func( gl::blend_style::SRC_ALPHA, gl::blend_style::ONE_MINUS_SRC_ALPHA );

	ogl.clear_color( _style.background_color() );
	ogl.clear();
	ogl.set_projection( gl::matrix4::ortho( 0, w, 0, h ) );

	if ( _widget )
	{
		in_context( [&,this]
		{
			_widget->layout_target()->compute_bounds();
			_widget->set_size( w, h );
			_widget->layout_target()->set_size( w, h );
			_widget->layout_target()->compute_layout();
			if ( _widget->update_layout( 250.0 ) )
				invalidate( *_widget );
			_widget->paint( *this );
		} );
	}

	hwctxt.swap_buffers();
}

////////////////////////////////////////

void window::resized( coord w, coord h )
{
	if ( _widget )
	{
		in_context( [&,this]
		{
			_widget->set( { coord(0), coord(0) }, { w, h } );
			_widget->layout_target()->set( { coord(0), coord(0) }, { w, h } );
			_widget->layout_target()->compute_layout();
			_widget->update_layout( 0.0 );
			invalidate( *_widget );
		} );
	}
}

////////////////////////////////////////

void window::mouse_press( const point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_press( p, b ); } );
}

////////////////////////////////////////

void window::mouse_release( const point &p, int b )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_release( p, b ); } );
}

////////////////////////////////////////

void window::mouse_moved( const point &p )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_move( p ); } );
}

////////////////////////////////////////

void window::mouse_wheel( int amount )
{
	if ( _widget )
		in_context( [&,this] { _widget->mouse_wheel( amount ); } );
}

////////////////////////////////////////

void window::key_pressed( platform::scancode c )
{
	if ( _widget )
		in_context( [&,this] { _widget->key_press( c ); } );
}

////////////////////////////////////////

void window::key_released( platform::scancode c )
{
	if ( _widget )
		in_context( [&,this] { _widget->key_release( c ); } );
}

////////////////////////////////////////

void window::text_entered( char32_t c )
{
	if ( _widget )
		in_context( [&,this] { _widget->text_input( c ); } );
}

////////////////////////////////////////

}

