
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include <core/pointer.h>
#include "dispatcher.h"

namespace xlib
{

////////////////////////////////////////

dispatcher::dispatcher( Display *dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _display( dpy ), _keyboard( k ), _mouse( m )
{
	precondition( _display, "null display" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	XFlush( _display );
	_exit_code = 0;

	bool done = false;
	XEvent event;
	while ( !done )
	{
		XNextEvent( _display, &event );
		switch ( event.type )
		{
			case Expose:
			{
				if ( event.xexpose.count == 0 )
					_windows[event.xexpose.window]->exposed();
				XFlush( _display );
				break;
			}

			case ConfigureNotify:
			{
				auto w = _windows[event.xconfigure.window];
				if ( w->check_last_position( event.xconfigure.x, event.xconfigure.y ) )
					w->moved( event.xconfigure.x, event.xconfigure.y );
				if ( w->check_last_size( event.xconfigure.width, event.xconfigure.height ) )
				{
					std::cout << "Configure notify! " << event.xconfigure.width << 'x' << event.xconfigure.height << std::endl;
					w->resize_canvas( event.xconfigure.width, event.xconfigure.height );
				}
				break;
			}

			case DestroyNotify:
			{
				auto w = _windows[event.xdestroywindow.window];
				w->closed();
				break;
			}

			case MapNotify:
			{
				auto w = _windows[event.xmap.window];
				w->shown();
				w->restored();
				break;
			}

			case UnmapNotify:
			{
				auto w = _windows[event.xunmap.window];
				w->hidden();
				w->minimized();
				break;
			}

			case EnterNotify:
			{
				auto w = _windows[event.xcrossing.window];
				w->entered();
				break;
			}

			case LeaveNotify:
			{
				auto w = _windows[event.xcrossing.window];
				w->exited();
				break;
			}

			case KeyPress:
			{
//				auto *ev = reinterpret_cast<xcb_key_press_event_t*>( event.get() );
//				auto w = _windows[ev->event];
//				platform::scancode sc = _keyboard->get_scancode( ev->detail );
//				w->key_pressed( _keyboard, sc );
				break;
			}

			case KeyRelease:
			{
//				auto *ev = reinterpret_cast<xcb_key_press_event_t*>( event.get() );
//				auto w = _windows[ev->event];
//				platform::scancode sc = _keyboard->get_scancode( ev->detail );
//				w->key_released( _keyboard, sc );
				break;
			}

			case MappingNotify:
			{
//				auto *ev = reinterpret_cast<xcb_mapping_notify_event_t*>( event.get() );
//				if ( ev->request == XCB_MAPPING_MODIFIER || ev->request == XCB_MAPPING_KEYBOARD )
//					_keyboard->update_mapping();
				break;
			}

			case ButtonPress:
			{
//				auto *ev = reinterpret_cast<xcb_button_press_event_t*>( event.get() );
//				auto w = _windows[ev->event];
//				switch ( ev->detail )
//				{
//					case 1:
//					case 2:
//					case 3:
//						w->mouse_pressed( _mouse, { double(ev->event_x), double(ev->event_y) }, ev->detail );
//						break;
//
//					case 4: // Mouse wheel up
//					case 5: // Mouse wheel down
//						break;
//				}
				break;
			}

			case ButtonRelease:
			{
//				auto *ev = reinterpret_cast<xcb_button_press_event_t*>( event.get() );
//				auto w = _windows[ev->event];
//				switch ( ev->detail )
//				{
//					case 1:
//					case 2:
//					case 3:
//						w->mouse_released( _mouse, { double(ev->event_x), double(ev->event_y) }, ev->detail );
//						break;
//
//					case 4: // Mouse wheel up
//					case 5: // Mouse wheel down
//						break;
//				}
				break;
			}

			case MotionNotify:
			{
//				auto *ev = reinterpret_cast<xcb_motion_notify_event_t*>( event.get() );
//				auto w = _windows[ev->event];
//				w->mouse_moved( _mouse, { double(ev->event_x), double(ev->event_y) } );
				break;
			}

			case VisibilityNotify:
			case ReparentNotify:
				break;

			case ClientMessage:
			{
//				auto *ev = reinterpret_cast<xcb_client_message_event_t*>( event.get() );
//				if ( ev->data.data32[0] == _atom_delete_window )
//					done = true;
				break;
			}

			default:
				std::cout << "Unknown event: " << uint32_t( event.type ) << std::endl;
		}
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	_windows[w->id()] = w;
//	xcb_change_property( _connection, XCB_PROP_MODE_REPLACE, w->id(), _atom_wm_protocols, XCB_ATOM_ATOM, 32, 1, &_atom_delete_window );
}

////////////////////////////////////////

}

