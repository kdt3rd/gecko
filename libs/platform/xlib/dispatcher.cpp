
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <X11/Xlib.h>
#include <base/contract.h>
#include <base/pointer.h>
#include <base/meta.h>
#include <utf/utf.h>
#include <utf/utfcat.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
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
		throw_runtime( "failed to open input method" );

	if ( pipe( _wait_pipe ) < 0 )
	{
		_wait_pipe[0] = -1;
		_wait_pipe[1] = -1;
		throw_errno( "Failed to create signaling pipe" );
	}
	if ( ::fcntl( _wait_pipe[0], F_SETFD, FD_CLOEXEC ) == -1 ||
		 ::fcntl( _wait_pipe[1], F_SETFD, FD_CLOEXEC ) == -1 )
		throw_errno( "Unable to convert signaling pipe to close-on-exec" );
	if ( ::fcntl( _wait_pipe[0], F_SETFL, O_NONBLOCK ) == -1 ||
		 ::fcntl( _wait_pipe[1], F_SETFL, O_NONBLOCK ) == -1 )
		throw_runtime( "Unable to convert signaling pipe to non blocking" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
	if ( _wait_pipe[0] >= 0 )
		::close( _wait_pipe[0] );
	if ( _wait_pipe[1] >= 0 )
		::close( _wait_pipe[1] );
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	XFlush( _display.get() );
	_exit_code = 0;
	_exit_requested.store( false );
	int xFD = ConnectionNumber( _display.get() );
	fd_set waitreadobjs;
	std::map<int, std::shared_ptr<waitable>> waitmap;
	std::vector<std::shared_ptr<waitable>> firenow;
	std::vector<std::shared_ptr<waitable>> timeouts;
	struct timeval tv;
	while ( ! _exit_requested.load() )
	{
		firenow.clear();
		waitmap.clear();
		timeouts.clear();
		int nWaits = std::max( _wait_pipe[0], xFD );
		FD_ZERO( &waitreadobjs );
		FD_SET( xFD, &waitreadobjs );
		if ( _wait_pipe[0] >= 0 )
			FD_SET( _wait_pipe[0], &waitreadobjs );

		waitable::time_point curt = waitable::clock::now();
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		struct timeval *tvptr = nullptr;
		for ( auto &w: _waitables )
		{
			intptr_t oid = w->poll_object();
			if ( oid != intptr_t(-1) )
			{
				int objfd = static_cast<int>( oid );
				waitmap[objfd] = w;
				FD_SET( objfd, &waitreadobjs );
				nWaits = std::max( nWaits, objfd );
			}
			waitable::duration when;
			if ( w->poll_timeout( when, curt ) )
			{
				if ( when < waitable::duration::zero() )
				{
					firenow.push_back( w );
				}
				else
				{
					timeouts.push_back( w );
					std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>( when );
					when -= std::chrono::duration_cast<waitable::duration>( secs );
					std::chrono::microseconds usecs = std::chrono::duration_cast<std::chrono::microseconds>( when );
					if ( secs.count() < tv.tv_sec || ( secs.count() == tv.tv_sec && usecs.count() < tv.tv_usec ) )
					{
						tv.tv_sec = secs.count();
						tv.tv_usec = usecs.count();
					}

					tvptr = &tv;
				}
			}
		}

		bool doTimeouts = false;
		if ( firenow.empty() )
		{
			++nWaits;
			int selrv = select( nWaits, &waitreadobjs, NULL, NULL, tvptr );

			// timeout if selrv == 0
			if ( selrv < 0 )
				throw_errno( "Error waiting for events to be available" );

			doTimeouts = ( selrv == 0 );
			for ( auto &x: waitmap )
			{
				if ( FD_ISSET( x.first, &waitreadobjs ) )
					firenow.push_back( x.second );
			}
			if ( FD_ISSET( _wait_pipe[0], &waitreadobjs ) )
			{
				char j;
				int rc;
				do
				{
					rc = ::read( _wait_pipe[0], &j, 1 );
				} while ( rc > 0 || ( rc == -1 && errno == EINTR ) );
			}
		}

		curt = waitable::clock::now();
		if ( doTimeouts )
		{
			for ( auto &w: timeouts )
				w->emit( curt );
		}
		for ( auto &w: firenow )
			w->emit( curt );
		if ( drain_xlib_events() )
			break;
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
	_exit_requested.store( true );
}

////////////////////////////////////////

void
dispatcher::add_waitable( const std::shared_ptr<waitable> &w )
{
	::platform::dispatcher::add_waitable( w );
	wake_up_executor();
}

////////////////////////////////////////

void
dispatcher::remove_waitable( const std::shared_ptr<waitable> &w )
{
	::platform::dispatcher::remove_waitable( w );
	wake_up_executor();
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
		throw_runtime( "failed to create input context" );
	w->set_input_context( xic );
}

////////////////////////////////////////

void
dispatcher::wake_up_executor( void )
{
	if ( _wait_pipe[1] >= 0 )
	{
		char x = 'x';
		::write( _wait_pipe[1], &x, sizeof(char) );
	}
}

////////////////////////////////////////

bool
dispatcher::drain_xlib_events( void )
{
	bool done = false;
	XEvent event;
	while ( ! done && XPending( _display.get() ) )
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
				if ( w->shown )
					w->shown();
				if ( w->restored )
					w->restored();
				break;
			}

			case UnmapNotify:
			{
				auto w = _windows[event.xunmap.window];
				if ( w )
				{
					if ( w->hidden )
						w->hidden();
					if ( w->minimized )
						w->minimized();
				}
				break;
			}

			case EnterNotify:
			{
				auto w = _windows[event.xcrossing.window];
				if ( w->entered )
					w->entered();
				break;
			}

			case LeaveNotify:
			{
				auto w = _windows[event.xcrossing.window];
				if ( w->exited )
					w->exited();
				break;
			}

			case KeyPress:
			{
				auto w = _windows[event.xkey.window];
				char keybuf[32];
				std::unique_ptr<char[]> tmpbuf;
				char *keyptr = keybuf;
				size_t keyptrlen = sizeof(keybuf);
				KeySym symbol;
				bool gotSym = false;
				int length = 0;
				bool cont = false;
				do
				{
					Status status;
					length = Xutf8LookupString( w->input_context(), &event.xkey, keybuf, sizeof(keybuf), &symbol, &status );
					switch ( status )
					{
						case XBufferOverflow:
							tmpbuf.reset( new char[keyptrlen*2]);
							keyptr = tmpbuf.get();
							keyptrlen = keyptrlen*2;
							cont = true;
							break;

						case XLookupNone:
							break;
						case XLookupChars:
							gotSym = false;
							break;
						case XLookupBoth:
							gotSym = true;
							break;
						case XLookupKeySym:
							gotSym = true;
							keyptr = nullptr;
							length = 0;
							break;
					}
				} while ( cont );
				
				if ( w->key_pressed )
				{
					platform::scancode sc;
					if ( gotSym )
						sc = _keyboard->get_scancode( event.xkey, symbol );
					else
						sc = _keyboard->get_scancode( event.xkey );
					w->key_pressed( _keyboard, sc );
				}

				if ( keyptr && length > 0 && w->text_entered )
				{
					std::stringstream tmp( std::string( keyptr, size_t(length) ) );
					utf::iterator it( tmp, utf::UTF8 );
					while ( ++it )
					{
						// why only graphic? Why not formatting as well?
						if ( utf::is_graphic( *it ) )
							w->text_entered( _keyboard, *it );
					}
				}
				break;
			}

			case KeyRelease:
			{
				auto w = _windows[event.xkey.window];
				if ( w->key_released )
				{
					platform::scancode sc = _keyboard->get_scancode( event.xkey );
					w->key_released( _keyboard, sc );
				}
				break;
			}

			case KeymapNotify:
			{
				// TODO: fill in currently depressed state as appropriate
				break;
			}

			case MappingNotify:
			{
				if ( event.xmapping.request == MappingKeyboard || event.xmapping.request == MappingModifier )
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
						if ( w->mouse_pressed )
							w->mouse_pressed( _mouse, { double(event.xbutton.x), double(event.xbutton.y) }, int(event.xbutton.button) );
						break;

					case 4:
						if ( w->mouse_wheel )
							w->mouse_wheel( _mouse, 1 );
						break;

					case 5:
						if ( w->mouse_wheel )
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
						if ( w->mouse_released )
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
				if ( w->mouse_moved )
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

	return done;
}

////////////////////////////////////////

} }

