
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <X11/Xlib.h>
#include <base/contract.h>
#include <base/pointer.h>
#include <base/meta.h>
#include <utf/utf.h>
#include <utf/utfcat.h>
#include "dispatcher.h"

namespace platform { namespace xlib
{

////////////////////////////////////////

dispatcher::dispatcher( const std::shared_ptr<Display> &dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _display( dpy ), _keyboard( k ), _mouse( m )
{
	precondition( _display, "null display" );
	_atom_delete_window = XInternAtom( _display.get(), "WM_DELETE_WINDOW", True );

	_xim = XOpenIM( _display.get(), nullptr, nullptr, nullptr ); // TODO resource/class name?
	if ( !_xim )
		throw std::runtime_error( "failed to open input method" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	XFlush( _display.get() );
	_exit_code = 0;

	bool done = false;
	XEvent event;
	while ( !done )
	{
		XNextEvent( _display.get(), &event );
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
				if ( w )
				{
					w->move_event( event.xconfigure.x, event.xconfigure.y );
					w->resize_event( event.xconfigure.width, event.xconfigure.height );
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
				if ( w )
				{
					w->hidden();
					w->minimized();
				}
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
				char keybuf[16];
				Status status;
				int length = Xutf8LookupString( w->input_context(), &event.xkey, keybuf, sizeof(keybuf), nullptr, &status );
				if ( length > 0 )
				{
					std::stringstream tmp( std::string( keybuf, size_t(length) ) );
					utf::iterator it( tmp, utf::UTF8 );
					while ( ++it )
					{
						if ( utf::is_graphic( *it ) )
							w->text_entered( _keyboard, *it );
					}
				}
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
						w->mouse_pressed( _mouse, { double(event.xbutton.x), double(event.xbutton.y) }, int(event.xbutton.button) );
						break;

					case 4:
						w->mouse_wheel( _mouse, 1 );
						break;

					case 5:
						w->mouse_wheel( _mouse, -1 );
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
						w->mouse_released( _mouse, { double(event.xbutton.x), double(event.xbutton.y) }, int(event.xbutton.button) );
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
	XSetWMProtocols( _display.get(), w->id(), &_atom_delete_window, 1 );

	// Create an input context.
	auto xic = XCreateIC( _xim,
		XNClientWindow, w->id(),
		XNFocusWindow, w->id(), 
		XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
		nullptr );
	if ( !xic )
		throw std::runtime_error( "failed to create input context" );
	w->set_input_context( xic );
}

////////////////////////////////////////

} }

