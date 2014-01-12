
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include <core/pointer.h>
#include <core/meta.h>
#include "dispatcher.h"

namespace xlib
{

////////////////////////////////////////

dispatcher::dispatcher( Display *dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _display( dpy ), _keyboard( k ), _mouse( m )
{
	precondition( _display, "null display" );
	_atom_delete_window = XInternAtom( _display, "WM_DELETE_WINDOW", True );
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
				{
					auto win = _windows[event.xexpose.window];
					win->expose_event();
				}
				break;
			}

			case ConfigureNotify:
			{
				auto w = _windows[event.xconfigure.window];
				w->move_event( event.xconfigure.x, event.xconfigure.y );
				w->resize_event( event.xconfigure.width, event.xconfigure.height );
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
				auto w = _windows[event.xkey.window];
				platform::scancode sc = _keyboard->get_scancode( event.xkey );
				w->key_pressed( _keyboard, sc );
				break;
			}

			case KeyRelease:
			{
				auto w = _windows[event.xkey.window];
				platform::scancode sc = _keyboard->get_scancode( event.xkey );
				w->key_released( _keyboard, sc );
				break;
			}

			case MappingNotify:
			{
				if ( event.xmapping.request == MappingKeyboard )
					XRefreshKeyboardMapping( &event.xmapping );
				break;
			}

			case ButtonPress:
			{
				auto w = _windows[event.xbutton.window];
				switch ( event.xbutton.button )
				{
					case 1:
					case 2:
					case 3:
						w->mouse_pressed( _mouse, { double(event.xbutton.x), double(event.xbutton.y) }, event.xbutton.button );
						break;

					case 4: // Mouse wheel up
					case 5: // Mouse wheel down
						break;
				}
				break;
			}

			case ButtonRelease:
			{
				auto w = _windows[event.xbutton.window];
				switch ( event.xbutton.button )
				{
					case 1:
					case 2:
					case 3:
						w->mouse_released( _mouse, { double(event.xbutton.x), double(event.xbutton.y) }, event.xbutton.button );
						break;

					case 4: // Mouse wheel up
					case 5: // Mouse wheel down
						break;
				}
				break;
			}

			case MotionNotify:
			{
				auto w = _windows[event.xmotion.window];
				w->mouse_moved( _mouse, { double(event.xmotion.x), double(event.xmotion.y) } );
				break;
			}

			case VisibilityNotify:
			case ReparentNotify:
				break;

			case ClientMessage:
			{
				auto w = _windows[event.xclient.window];
				if ( Atom(event.xclient.data.l[0]) == _atom_delete_window )
				{
					w->hide();
					_windows.erase( w->id() );
					done = _windows.empty();
				}
				break;
			}

			case 65:
				// TODO bug in cairo??
				// I think this is a XShm event?
				// Ignore for now...
				break;

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
	XSetWMProtocols( _display, w->id(), &_atom_delete_window, 1 );
}

////////////////////////////////////////

}

