
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include <core/pointer.h>
#include "dispatcher.h"
#include "atom.h"
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

namespace xcb
{

////////////////////////////////////////

dispatcher::dispatcher( xcb_connection_t *c, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _connection( c ), _keyboard( k ), _mouse( m )
{
	precondition( _connection, "null connection" );

	_atom_wm_protocols = get_atom( _connection, "WM_PROTOCOLS" );
	_atom_delete_window = get_atom( _connection, "WM_DELETE_WINDOW" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	xcb_flush( _connection );
	_exit_code = 0;

	bool done = false;
	while ( !done )
	{
		auto event = core::wrap_cptr( xcb_wait_for_event( _connection ) );
		if ( !event )
			break;
		switch ( event->response_type & ~0x80 )
		{
			case XCB_EXPOSE:
			{
				auto *ev = reinterpret_cast<xcb_expose_event_t*>( event.get() );
				if ( ev->count == 0 )
					_windows[ev->window]->exposed();
				xcb_flush( _connection );
				break;
			}

			case XCB_CONFIGURE_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_configure_notify_event_t*>( event.get() );
				auto w = _windows[ev->window];
				if ( w->check_last_position( ev->x, ev->y ) )
					w->moved( ev->x, ev->y );
				if ( w->check_last_size( ev->width, ev->height ) )
					w->resized( ev->width, ev->height );
				break;
			}

			case XCB_DESTROY_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_destroy_notify_event_t*>( event.get() );
				auto w = _windows[ev->window];
				w->closed();
				break;
			}

			case XCB_MAP_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_map_notify_event_t*>( event.get() );
				auto w = _windows[ev->window];
				w->shown();
				w->restored();
				break;
			}

			case XCB_UNMAP_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_unmap_notify_event_t*>( event.get() );
				auto w = _windows[ev->window];
				w->hidden();
				w->minimized();
				break;
			}

			case XCB_ENTER_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_enter_notify_event_t*>( event.get() );
				auto w = _windows[ev->event];
				w->entered();
				break;
			}

			case XCB_LEAVE_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_leave_notify_event_t*>( event.get() );
				auto w = _windows[ev->event];
				w->exited();
				break;
			}

			case XCB_KEY_PRESS:
			{
				auto *ev = reinterpret_cast<xcb_key_press_event_t*>( event.get() );
				auto w = _windows[ev->event];
				platform::scancode sc = _keyboard->get_scancode( ev->detail );
				w->key_pressed( _keyboard, sc );
				break;
			}

			case XCB_KEY_RELEASE:
			{
				auto *ev = reinterpret_cast<xcb_key_press_event_t*>( event.get() );
				auto w = _windows[ev->event];
				platform::scancode sc = _keyboard->get_scancode( ev->detail );
				w->key_released( _keyboard, sc );
				break;
			}

			case XCB_MAPPING_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_mapping_notify_event_t*>( event.get() );
				if ( ev->request == XCB_MAPPING_MODIFIER || ev->request == XCB_MAPPING_KEYBOARD )
					_keyboard->update_mapping();
				break;
			}

			case XCB_BUTTON_PRESS:
			{
				auto *ev = reinterpret_cast<xcb_button_press_event_t*>( event.get() );
				auto w = _windows[ev->event];
				switch ( ev->detail )
				{
					case 1:
					case 2:
					case 3:
						w->mouse_pressed( _mouse, { double(ev->event_x), double(ev->event_y) }, ev->detail );
						break;

					case 4: // Mouse wheel up
					case 5: // Mouse wheel down
						break;
				}
				break;
			}

			case XCB_BUTTON_RELEASE:
			{
				auto *ev = reinterpret_cast<xcb_button_press_event_t*>( event.get() );
				auto w = _windows[ev->event];
				switch ( ev->detail )
				{
					case 1:
					case 2:
					case 3:
						w->mouse_released( _mouse, { double(ev->event_x), double(ev->event_y) }, ev->detail );
						break;

					case 4: // Mouse wheel up
					case 5: // Mouse wheel down
						break;
				}
				break;
			}

			case XCB_MOTION_NOTIFY:
			{
				auto *ev = reinterpret_cast<xcb_motion_notify_event_t*>( event.get() );
				auto w = _windows[ev->event];
				w->mouse_moved( _mouse, { double(ev->event_x), double(ev->event_y) } );
				break;
			}

			case XCB_VISIBILITY_NOTIFY:
			case XCB_REPARENT_NOTIFY:
				break;

			case XCB_CLIENT_MESSAGE:
			{
				auto *ev = reinterpret_cast<xcb_client_message_event_t*>( event.get() );
				std::cout << "CLIENT MESSAGE! " << ev->data.data32[0] << std::endl;
				if ( ev->data.data32[0] == _atom_delete_window )
					done = true;
				break;
			}

			default:
				std::cout << "Unknown event: " << uint32_t( event->response_type & ~0x80 ) << ' ' << uint32_t( event->response_type ) << std::endl;
		}
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;

	/*
	SDL_Event event;
	event.type = SDL_QUIT;
	if ( SDL_PushEvent( &event) == 0 )
		throw std::runtime_error( SDL_GetError() );
	*/
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	_windows[w->id()] = w;
	xcb_change_property( _connection, XCB_PROP_MODE_REPLACE, w->id(), _atom_wm_protocols, XCB_ATOM_ATOM, 32, 1, &_atom_delete_window );
}

////////////////////////////////////////

}

