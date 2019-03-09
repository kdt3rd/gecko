// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <base/contract.h>
#include <base/pointer.h>
#include <base/meta.h>
#include <base/scope_guard.h>
#include <utf/utf.h>
#include <utf/utfcat.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include "dispatcher.h"
#include "keyboard.h"
#include "mouse.h"
#include <platform/event_queue.h>
#include "system.h"
#include "screen.h"
#include "context.h"

////////////////////////////////////////

namespace
{

static void ximShutdown( XIM, XPointer p, XPointer d )
{
	// TODO: add proper logging support
	std::cerr << "XIM server shutdown / died" << std::endl;
	if ( p )
	{
		XIM *ptr = reinterpret_cast<XIM *>( p );
		*ptr = nullptr;
	}
}

platform::scancode get_scancode( XKeyEvent &ev, KeySym symbol )
{
	// hrm, there is no good way to go from XKeyEvent.key_code to an actual
	// scancode
	static const std::map<KeySym,platform::scancode> sym2code =
		{
			{ XK_Return, platform::scancode::KEY_RETURN },
			{ XK_Escape, platform::scancode::KEY_ESCAPE },
			{ XK_BackSpace, platform::scancode::KEY_BACKSPACE },
			{ XK_Tab, platform::scancode::KEY_TAB },
			{ XK_space, platform::scancode::KEY_SPACE },
			{ XK_minus, platform::scancode::KEY_MINUS },
			{ XK_equal, platform::scancode::KEY_EQUAL },
			{ XK_plus, platform::scancode::KEY_EQUAL },
			{ XK_bracketleft, platform::scancode::KEY_LEFTBRACKET },
			{ XK_braceleft, platform::scancode::KEY_LEFTBRACKET },
			{ XK_bracketright, platform::scancode::KEY_RIGHTBRACKET },
			{ XK_braceright, platform::scancode::KEY_RIGHTBRACKET },
			{ XK_slash, platform::scancode::KEY_BACKSLASH },
			{ XK_semicolon, platform::scancode::KEY_SEMICOLON },
			{ XK_apostrophe, platform::scancode::KEY_APOSTROPHE },
			{ XK_quotedbl, platform::scancode::KEY_APOSTROPHE },
			{ XK_grave, platform::scancode::KEY_GRAVE },
			{ XK_asciitilde, platform::scancode::KEY_GRAVE },
			{ XK_comma, platform::scancode::KEY_COMMA },
			{ XK_less, platform::scancode::KEY_COMMA },
			{ XK_period, platform::scancode::KEY_PERIOD },
			{ XK_greater, platform::scancode::KEY_PERIOD },
			{ XK_slash, platform::scancode::KEY_SLASH },
			{ XK_question, platform::scancode::KEY_SLASH },
			{ XK_Caps_Lock, platform::scancode::KEY_CAPSLOCK },
			{ XK_exclam, platform::scancode::KEY_1 },
			{ XK_at, platform::scancode::KEY_2 },
			{ XK_numbersign, platform::scancode::KEY_3 },
			{ XK_dollar, platform::scancode::KEY_4 },
			{ XK_percent, platform::scancode::KEY_5 },
			{ XK_asciicircum, platform::scancode::KEY_6 },
			{ XK_ampersand, platform::scancode::KEY_7 },
			{ XK_asterisk, platform::scancode::KEY_8 },
			{ XK_parenleft, platform::scancode::KEY_9 },
			{ XK_parenright, platform::scancode::KEY_0 },
			{ XK_F1, platform::scancode::KEY_F1 },
			{ XK_F2, platform::scancode::KEY_F2 },
			{ XK_F3, platform::scancode::KEY_F3 },
			{ XK_F4, platform::scancode::KEY_F4 },
			{ XK_F5, platform::scancode::KEY_F5 },
			{ XK_F6, platform::scancode::KEY_F6 },
			{ XK_F7, platform::scancode::KEY_F7 },
			{ XK_F8, platform::scancode::KEY_F8 },
			{ XK_F9, platform::scancode::KEY_F9 },
			{ XK_F10, platform::scancode::KEY_F10 },
			{ XK_F11, platform::scancode::KEY_F11 },
			{ XK_F12, platform::scancode::KEY_F12 },
			{ XK_Sys_Req, platform::scancode::KEY_PRINTSCREEN },
			{ XK_Scroll_Lock, platform::scancode::KEY_SCROLLLOCK},
			{ XK_Pause, platform::scancode::KEY_PAUSE },
			{ XK_Insert, platform::scancode::KEY_INSERT},
			{ XK_Home, platform::scancode::KEY_HOME},
			{ XK_Page_Up, platform::scancode::KEY_PAGEUP},
			{ XK_Delete, platform::scancode::KEY_DELETE},
			{ XK_End, platform::scancode::KEY_END },
			{ XK_Page_Down, platform::scancode::KEY_PAGEDOWN},
			{ XK_Right, platform::scancode::KEY_RIGHT },
			{ XK_Left, platform::scancode::KEY_LEFT},
			{ XK_Down, platform::scancode::KEY_DOWN},
			{ XK_Up, platform::scancode::KEY_UP},
			{ XK_Num_Lock, platform::scancode::KEY_NUMLOCK },
			{ XK_backslash, platform::scancode::KEY_BACKSLASH },
			{ XK_bar, platform::scancode::KEY_KP_PIPE },
			{ XK_underscore, platform::scancode::KEY_MINUS },
			{ XK_KP_Divide, platform::scancode::KEY_KP_DIVIDE },
			{ XK_KP_Multiply, platform::scancode::KEY_KP_MULTIPLY },
			{ XK_KP_Subtract, platform::scancode::KEY_KP_MINUS},
			{ XK_KP_Add, platform::scancode::KEY_KP_PLUS },
			{ XK_KP_Enter, platform::scancode::KEY_KP_ENTER},
			{ XK_KP_End, platform::scancode::KEY_KP_1 },
			{ XK_KP_Down, platform::scancode::KEY_KP_2 },
			{ XK_KP_Page_Down, platform::scancode::KEY_KP_3 },
			{ XK_KP_Left, platform::scancode::KEY_KP_4 },
			{ XK_KP_Begin, platform::scancode::KEY_KP_5 },
			{ XK_KP_Right, platform::scancode::KEY_KP_6 },
			{ XK_KP_Home, platform::scancode::KEY_KP_7 },
			{ XK_KP_Up, platform::scancode::KEY_KP_8 },
			{ XK_KP_Page_Up, platform::scancode::KEY_KP_9 },
			{ XK_KP_Insert, platform::scancode::KEY_KP_0 },
			{ XK_KP_Delete, platform::scancode::KEY_KP_PERIOD },
//	{ XK_, platform::scancode::KEY_APPLICATION },
//	{ XK_, platform::scancode::KEY_POWER },
			{ XK_KP_Equal, platform::scancode::KEY_KP_EQUAL},
			{ XK_F13, platform::scancode::KEY_F13 },
			{ XK_F14, platform::scancode::KEY_F14 },
			{ XK_F15, platform::scancode::KEY_F15 },
			{ XK_F16, platform::scancode::KEY_F16 },
			{ XK_F17, platform::scancode::KEY_F17 },
			{ XK_F18, platform::scancode::KEY_F18 },
			{ XK_F19, platform::scancode::KEY_F19 },
			{ XK_F20, platform::scancode::KEY_F20 },
			{ XK_F21, platform::scancode::KEY_F21 },
			{ XK_F22, platform::scancode::KEY_F22 },
			{ XK_F23, platform::scancode::KEY_F23 },
			{ XK_F24, platform::scancode::KEY_F24 },
//	{ XK_Execute, platform::scancode::KEY_EXEC},
			{ XK_Help, platform::scancode::KEY_HELP },
			{ XK_Menu, platform::scancode::KEY_MENU },
			{ XK_Select, platform::scancode::KEY_SELECT },
			{ XK_Cancel, platform::scancode::KEY_STOP },
			{ XK_Redo, platform::scancode::KEY_AGAIN},
			{ XK_Undo, platform::scancode::KEY_UNDO },
//	{ XK_, platform::scancode::KEY_CUT},
//	{ XK_, platform::scancode::KEY_COPY },
//	{ XK_, platform::scancode::KEY_PASTE},
			{ XK_Find, platform::scancode::KEY_FIND },
//	{ XK_, platform::scancode::KEY_MUTE },
//	{ XK_, platform::scancode::KEY_VOLUMEUP },
//	{ XK_, platform::scancode::KEY_VOLUMEDOWN },
			{ XK_Control_L, platform::scancode::KEY_LCTRL },
			{ XK_Shift_L, platform::scancode::KEY_LSHIFT },
			{ XK_Alt_L, platform::scancode::KEY_LALT },
			{ XK_Meta_L, platform::scancode::KEY_LGUI },
			{ XK_Super_L, platform::scancode::KEY_LGUI },
			{ XK_Control_R, platform::scancode::KEY_RCTRL},
			{ XK_Shift_R, platform::scancode::KEY_RSHIFT },
			{ XK_Alt_R, platform::scancode::KEY_RALT },
			{ XK_Meta_R, platform::scancode::KEY_RGUI },
			{ XK_Super_R, platform::scancode::KEY_RGUI },

			{ XF86XK_AudioLowerVolume, platform::scancode::KEY_VOLUMEDOWN },
			{ XF86XK_AudioMute, platform::scancode::KEY_MUTE },
			{ XF86XK_AudioRaiseVolume, platform::scancode::KEY_VOLUMEUP },
			{ XF86XK_AudioPlay, platform::scancode::KEY_PLAY_TOGGLE },
			{ XF86XK_AudioStop, platform::scancode::KEY_PLAY_STOP },
			{ XF86XK_AudioPrev, platform::scancode::KEY_PLAY_PREV_TRACK },
			{ XF86XK_AudioNext, platform::scancode::KEY_PLAY_NEXT_TRACK },
		};

	if ( symbol >= XK_a && symbol <= XK_z )
		return static_cast<platform::scancode>( static_cast<uint32_t>(platform::scancode::KEY_A) + ( symbol - XK_a ) );

	if ( symbol >= XK_A && symbol <= XK_Z )
		return static_cast<platform::scancode>( static_cast<uint32_t>(platform::scancode::KEY_A) + ( symbol - XK_A ) );

	if ( symbol >= XK_0 && symbol <= XK_9 )
		return static_cast<platform::scancode>( static_cast<uint32_t>(platform::scancode::KEY_0) + ( symbol - XK_0 ) );

	auto c = sym2code.find( symbol );
	if ( c != sym2code.end() )
		return c->second;

	std::cout << "WARNING, unknown keysym: 0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << symbol << std::dec << std::endl;
	return platform::scancode::KEY_UNKNOWN;
}

platform::scancode get_scancode( XKeyEvent &ev )
{
	KeySym symbol;
	char buffer[32];
	XLookupString( &ev, buffer, sizeof(buffer), &symbol, nullptr );
	if ( symbol != NoSymbol )
		return get_scancode( ev, symbol );

	return platform::scancode::KEY_UNKNOWN;
}

} // empty namespace

namespace platform { namespace xlib
{

////////////////////////////////////////

dispatcher::dispatcher( ::platform::system *sys, const std::shared_ptr<Display> &dpy )
	: ::platform::dispatcher( sys ), _system( sys ), _wait_pipe( true, false ), _display( dpy ),
	  _keyboard( std::make_shared<keyboard>( sys ) ),
	  _mouse( std::make_shared<mouse>( sys ) )
{
	// TODO: look at using libinput even for xlib?
	// don't have a good way to identify individual keyboards / mice
	// in raw xlib? maybe it doesn't matter...

	for ( auto &x: _dispatch )
		x = &dispatcher::dispatchUNKNOWN;
	// from X.h
	_dispatch[KeyPress] = &dispatcher::dispatchKeyPress;
	_dispatch[KeyRelease] = &dispatcher::dispatchKeyRelease;
	_dispatch[ButtonPress] = &dispatcher::dispatchButtonPress;
	_dispatch[ButtonRelease] = &dispatcher::dispatchButtonRelease;
	_dispatch[MotionNotify] = &dispatcher::dispatchMotionNotify;
	_dispatch[EnterNotify] = &dispatcher::dispatchEnterNotify;
	_dispatch[LeaveNotify] = &dispatcher::dispatchLeaveNotify;
	_dispatch[FocusIn] = &dispatcher::dispatchFocusIn;
	_dispatch[FocusOut] = &dispatcher::dispatchFocusOut;
	_dispatch[KeymapNotify] = &dispatcher::dispatchKeymapNotify;
	_dispatch[Expose] = &dispatcher::dispatchExpose;
	_dispatch[GraphicsExpose] = &dispatcher::dispatchGraphicsExpose;
	_dispatch[NoExpose] = &dispatcher::dispatchNoExpose;
	_dispatch[VisibilityNotify] = &dispatcher::dispatchVisibilityNotify;
	_dispatch[CreateNotify] = &dispatcher::dispatchCreateNotify;
	_dispatch[DestroyNotify] = &dispatcher::dispatchDestroyNotify;
	_dispatch[UnmapNotify] = &dispatcher::dispatchUnmapNotify;
	_dispatch[MapNotify] = &dispatcher::dispatchMapNotify;
	_dispatch[MapRequest] = &dispatcher::dispatchMapRequest;
	_dispatch[ReparentNotify] = &dispatcher::dispatchReparentNotify;
	_dispatch[ConfigureNotify] = &dispatcher::dispatchConfigureNotify;
	_dispatch[ConfigureRequest] = &dispatcher::dispatchConfigureRequest;
	_dispatch[GravityNotify] = &dispatcher::dispatchGravityNotify;
	_dispatch[ResizeRequest] = &dispatcher::dispatchResizeRequest;
	_dispatch[CirculateNotify] = &dispatcher::dispatchCirculateNotify;
	_dispatch[CirculateRequest] = &dispatcher::dispatchCirculateRequest;
	_dispatch[PropertyNotify] = &dispatcher::dispatchPropertyNotify;
	_dispatch[SelectionClear] = &dispatcher::dispatchSelectionClear;
	_dispatch[SelectionRequest] = &dispatcher::dispatchSelectionRequest;
	_dispatch[SelectionNotify] = &dispatcher::dispatchSelectionNotify;
	_dispatch[ColormapNotify] = &dispatcher::dispatchColormapNotify;
	_dispatch[ClientMessage] = &dispatcher::dispatchClientMessage;
	_dispatch[MappingNotify] = &dispatcher::dispatchMappingNotify;
	_dispatch[GenericEvent] = &dispatcher::dispatchGenericEvent;
	
	precondition( _display, "null display" );
	auto disp = _display.get();
	int screen = DefaultScreen( disp );
	Window root = RootWindow( disp, screen );

	// set up auto repeat same as os/x & win32
	int xkbOp, xkbEv, xkbErr;
	int xkbMaj = XkbMajorVersion, xkbMin = XkbMinorVersion;
	if ( XkbQueryExtension( disp, &xkbOp, &xkbEv, &xkbErr, &xkbMaj, &xkbMin ) == True )
		XkbSetDetectableAutoRepeat( disp, true, nullptr );

	_atom_delete_window = XInternAtom( disp, "WM_DELETE_WINDOW", True );
	_atom_quit_app = XInternAtom( disp, "_WM_QUIT_APP", True );

	_clipboard_win = XCreateSimpleWindow( disp, root, 0, 0, 100, 100, 0, BlackPixel( disp, screen ), BlackPixel( disp, screen ) );
	if ( _clipboard_win == 0 )
		throw_runtime( "unable to create clipboard handling window" );

	XSetLocaleModifiers("");
	char buf[10];
	strcpy( buf, "Gecko" ); // TODO resource/class name?
	_xim = XOpenIM( _display.get(), nullptr, buf, buf );
	if ( !_xim )
		throw_runtime( "failed to open input method" );

	XIMCallback shutdown;
	shutdown.callback = (XIMProc)ximShutdown;
	shutdown.client_data = (XPointer)&_xim;
	XSetIMValues( _xim, XNDestroyCallback, &shutdown, nullptr );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
	for ( auto w: _windows )
		remove_window( w.second );
	_windows.clear();

	XDestroyWindow( _display.get(), _clipboard_win );
	if ( _xim )
		XCloseIM( _xim );
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	XFlush( _display.get() );
	_exit_code = 0;
	_exit_requested.store( false );
	run_event_loop_until( &_exit_requested );

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
	Atom protocols[2] = { _atom_delete_window, _atom_quit_app };
	XSetWMProtocols( _display.get(), w->id(), protocols, 2 );

	// Create an input context.
	if ( _xim )
	{
		auto xic = XCreateIC( _xim,
							  XNClientWindow, w->id(),
							  XNFocusWindow, w->id(), 
							  XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
							  nullptr );
		if ( !xic )
			throw_runtime( "failed to create input context" );
		w->set_input_context( xic );
	}
}

////////////////////////////////////////

void
dispatcher::remove_window( const std::shared_ptr<window> &w )
{
	if ( _xim )
		XUnsetICFocus( w->input_context() );

	w->hide();

	_windows.erase( w->id() );
}

////////////////////////////////////////

void
dispatcher::set_selection( selection sel )
{
	initSelectionAtoms();

	_sel = std::move( sel );
	// TBD: do we need this ever, or are we always time independent?
	_sel_time = CurrentTime;
	Atom selA = getSelectionAtom( _sel.which_clip(), _sel.custom_clipboard() );
	_sel_avail_types.clear();
	_sel_avail_types.reserve( sel.available_mime_types().size() );
	for ( auto &mt: sel.available_mime_types() )
	{
		Atom mta;
		auto mti = _sel_type_pool.find( mt );
		if ( mti == _sel_type_pool.end() )
		{
			mta = XInternAtom( _display.get(), mt.c_str(), False );
			_sel_type_pool[mt] = mta;
		}
		else
		{
			mta = mti->second;
		}
		_sel_avail_types.push_back( mta );
	}

	XSetSelectionOwner( _display.get(), selA, _clipboard_win, _sel_time );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
dispatcher::query_selection( selection_type sel,
							 const std::vector<std::string> &allowedTypes,
							 const std::string &clipboardName )
{
	initSelectionAtoms();
	Atom selA = getSelectionAtom( sel, clipboardName );

	std::vector<uint8_t> r;
	std::string rtype;
	if ( _sel_targets != None && selA != None )
	{
		std::atomic<bool> fin{ false };
		_sel_stack.emplace( &r, &rtype, &allowedTypes, &fin, selA );
		on_scope_exit{ _sel_stack.pop(); };

		XConvertSelection( _display.get(), selA, _sel_targets, selA, _clipboard_win, CurrentTime );
		run_event_loop_until( &fin );
	}
	else
		throw_runtime( "Unable to retrieve clipboard atoms" );

	return std::make_pair( std::move( r ), std::move( rtype ) );
}

////////////////////////////////////////

std::pair<std::vector<uint8_t>, std::string>
dispatcher::query_selection( selection_type sel,
							 const selection_type_function &chooseMimeType,
							 const std::string &clipboardName )
{
	initSelectionAtoms();
	Atom selA = getSelectionAtom( sel, clipboardName );

	std::vector<uint8_t> r;
	std::string rtype;
	if ( _sel_targets != None && selA != None )
	{
		std::atomic<bool> fin{ false };
		_sel_stack.emplace( &r, &rtype, &chooseMimeType, &fin, selA );
		on_scope_exit{ _sel_stack.pop(); };

		XConvertSelection( _display.get(), selA, _sel_targets, selA, _clipboard_win, CurrentTime );
		run_event_loop_until( &fin );
	}
	else
		throw_runtime( "Unable to retrieve clipboard atoms" );

	return std::make_pair( std::move( r ), std::move( rtype ) );
}

////////////////////////////////////////

context *
dispatcher::get_share_context( void ) const
{
	if ( _windows.empty() )
		return nullptr;

	auto i = _windows.begin();
	::platform::context *ctxt = &( i->second->hw_context() );
	return dynamic_cast<context *>( ctxt );
}

////////////////////////////////////////

void
dispatcher::wake_up_executor( void )
{
	_wait_pipe.write( "x", 1 );
}

////////////////////////////////////////

void
dispatcher::run_event_loop_until( std::atomic<bool> *end )
{
	int xFD = ConnectionNumber( _display.get() );
	fd_set waitreadobjs;
	std::map<int, std::shared_ptr<waitable>> waitmap;
	std::vector<std::shared_ptr<waitable>> firenow;
	std::vector<std::shared_ptr<waitable>> timeouts;
	struct timeval tv;
	while ( ! end->load(std::memory_order_relaxed) )
	{
		firenow.clear();
		waitmap.clear();
		timeouts.clear();
		int wpFD = _wait_pipe.readable().waitable();
		int nWaits = std::max( wpFD, xFD );
		FD_ZERO( &waitreadobjs );
		FD_SET( xFD, &waitreadobjs );
		FD_SET( wpFD, &waitreadobjs );

		waitable::time_point curt = waitable::clock::now();
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		struct timeval *tvptr = nullptr;
		for ( auto &w: _waitables )
		{
			int oid = w->poll_object().waitable();
			if ( oid != -1 )
			{
				waitmap[oid] = w;
				FD_SET( oid, &waitreadobjs );
				nWaits = std::max( nWaits, oid );
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
			if ( FD_ISSET( wpFD, &waitreadobjs ) )
			{
				char j;
				_wait_pipe.read( &j, 1 );
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
}

////////////////////////////////////////

bool
dispatcher::drain_xlib_events( void )
{
	XEvent event;
	std::shared_ptr<window> w;
	while ( XPending( _display.get() ) )
	{
		XNextEvent( _display.get(), &event );
		if ( event.type >= LASTEvent )
		{
			if ( event.type == screen::resolution_event_id() )
				dispatchRandRChange( event );
			continue;
		}
		

		if ( XFilterEvent( &event, None ) == True && event.type != KeyRelease )
			continue;

		auto x = _windows.find( event.xany.window );
		if ( x != _windows.end() )
			w = x->second;
		else
			w.reset();

		(this->*_dispatch[event.type])( w, event );
	}

	return _windows.empty();
}

////////////////////////////////////////

dispatcher::Property
dispatcher::read_property( Atom sel )
{
	Property r;
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *ret = nullptr;

	int read_bytes = 1024;
	do
	{
		if ( ret != nullptr )
		{
			XFree( ret );
			ret = nullptr;
		}
		XGetWindowProperty( _display.get(), _clipboard_win, sel, 0, read_bytes, False, AnyPropertyType,
							&actual_type, &actual_format, &nitems, &bytes_after, &ret );
		read_bytes *= 2;
	} while ( bytes_after != 0 );

	r.data = ret;
	r.format = actual_format;
	r.nitems = nitems;
	r.type = actual_type;
	std::cout << "read property: fmt " << r.format << " nitems " << r.nitems << " type " << XGetAtomName( _display.get(), r.type ) << std::endl;
	return r;
}

////////////////////////////////////////

void
dispatcher::initSelectionAtoms( void )
{
	if ( _sel_targets == None )
		_sel_targets = XInternAtom( _display.get(), "TARGETS", False );
	if ( _sel_multiple == None )
		_sel_multiple = XInternAtom( _display.get(), "MULTIPLE", False );
	if ( _sel_incr == None )
		_sel_incr = XInternAtom( _display.get(), "INCR", False );
	if ( _sel_timestamp == None )
		_sel_timestamp = XInternAtom( _display.get(), "TIMESTAMP", False );
	if ( _sel_primary == None )
		_sel_primary = XInternAtom( _display.get(), "PRIMARY", False );
	if ( _sel_clip == None )
		_sel_clip = XInternAtom( _display.get(), "CLIPBOARD", False );
}

////////////////////////////////////////

Atom
dispatcher::getSelectionAtom( selection_type sel, const std::string &clipboardName )
{
	Atom selA = None;
	switch ( sel )
	{
		case selection_type::MOUSE: selA = _sel_primary; break;
		case selection_type::CLIPBOARD: selA = _sel_clip; break;
		case selection_type::CUSTOM:
		{
			precondition( ! clipboardName.empty(), "valid clipboard name required for custom selection type" );
			auto i = _sel_custom_clips.find( clipboardName );
			if ( i == _sel_custom_clips.end() )
			{
				selA = XInternAtom( _display.get(), clipboardName.c_str(), False );
				_sel_custom_clips[clipboardName] = selA;
			}
			else
				selA = i->second;
			break;
		}
	}
	return selA;
}


////////////////////////////////////////

void dispatcher::dispatchKeyPress( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( ! w )
		return;

	if ( _xim )
	{
		XSetICFocus( w->input_context() );
		if ( XFilterEvent( &event, None ) == True )
			return;
	}

	char keybuf[32];
	std::unique_ptr<char[]> tmpbuf;
	char *keyptr = keybuf;
	size_t keyptrlen = sizeof(keybuf);
	KeySym symbol;
	bool gotSym = false;
	int length = 0;
	do
	{
		Status status;
		length = Xutf8LookupString( w->input_context(), &event.xkey, keyptr, sizeof(keyptrlen), &symbol, &status );
		switch ( status )
		{
			case XBufferOverflow:
				tmpbuf.reset( new char[keyptrlen*2] );
				keyptr = tmpbuf.get();
				keyptrlen = keyptrlen*2;
				continue;

			case XLookupNone:
			case XLookupChars:
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
	} while ( false );

	// TODO: Track multiple keys down...
	platform::scancode sc;
	if ( gotSym )
		sc = get_scancode( event.xkey, symbol );
	else
		sc = get_scancode( event.xkey );

	// TODO: track all modifier keys...
	uint8_t mods = ( (((event.xkey.state & ControlMask) != 0) ? 0x01 : 0) |
					 (((event.xkey.state & ShiftMask) != 0) ? 0x02 : 0) );

	w->process_event( event::key( _keyboard.get(),
								  event_type::KEYBOARD_DOWN,
								  event.xkey.x, event.xkey.y,
								  sc, mods ) );

	if ( keyptr && length > 0 )
	{
		std::stringstream tmp( std::string( keyptr, size_t(length) ) );
		utf::iterator it( tmp, utf::UTF8 );
		while ( ++it )
		{
			// why only graphic? Why not formatting as well?
			if ( utf::is_graphic( *it ) )
				w->process_event( event::text( _keyboard.get(),
											   event_type::TEXT_ENTERED,
											   event.xkey.x, event.xkey.y,
											   *it, mods ) );
		}
	}
}

////////////////////////////////////////

void dispatcher::dispatchKeyRelease( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		uint8_t mods = ( (((event.xkey.state & ControlMask) != 0) ? 0x01 : 0) |
						 (((event.xkey.state & ShiftMask) != 0) ? 0x02 : 0) );

		w->process_event( event::key( _keyboard.get(),
									  event_type::KEYBOARD_UP,
									  event.xkey.x, event.xkey.y,
									  get_scancode( event.xkey ), mods ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchButtonPress( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		uint8_t mods = ( (((event.xbutton.state & ControlMask) != 0) ? 0x01 : 0) |
						 (((event.xbutton.state & ShiftMask) != 0) ? 0x02 : 0) );

		if ( event.xbutton.button == 4 )
			w->process_event( event::hid( _mouse.get(), event_type::MOUSE_WHEEL,
										  event.xbutton.x, event.xbutton.y, 4, 1, mods ) );
		else if ( event.xbutton.button == 5 )
			w->process_event( event::hid( _mouse.get(), event_type::MOUSE_WHEEL,
										  event.xbutton.x, event.xbutton.y, 5, -1, mods ) );
		else
			w->process_event( event::mouse( _mouse.get(),
											event_type::MOUSE_DOWN,
											event.xbutton.x, event.xbutton.y,
											int(event.xbutton.button), mods ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchButtonRelease( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		if ( event.xbutton.button != 4 && event.xbutton.button != 5 )
		{
			uint8_t mods = ( (((event.xbutton.state & ControlMask) != 0) ? 0x01 : 0) |
							 (((event.xbutton.state & ShiftMask) != 0) ? 0x02 : 0) );
			w->process_event( event::mouse( _mouse.get(),
											event_type::MOUSE_UP,
											event.xbutton.x, event.xbutton.y,
											int(event.xbutton.button), mods ) );
		}
	}
}

////////////////////////////////////////

void dispatcher::dispatchMotionNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		uint8_t mods = ( (((event.xbutton.state & ControlMask) != 0) ? 0x01 : 0) |
						 (((event.xbutton.state & ShiftMask) != 0) ? 0x02 : 0) );
		w->process_event( event::mouse( _mouse.get(), event_type::MOUSE_MOVE,
										event.xbutton.x, event.xbutton.y, 0, mods ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchEnterNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	// skip virtual crossings
	if ( event.xcrossing.detail == NotifyVirtual ||
		 event.xcrossing.detail == NotifyNonlinearVirtual )
		return;

	if ( w )
	{
		uint8_t mods = ( (((event.xcrossing.state & ControlMask) != 0) ? 0x01 : 0) |
						 (((event.xcrossing.state & ShiftMask) != 0) ? 0x02 : 0) );
		_ext_events->grab( w );
		w->process_event( event::mouse( _mouse.get(), event_type::MOUSE_ENTER,
										event.xcrossing.x, event.xcrossing.y, 0, mods ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchLeaveNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	// skip virtual crossings
	if ( event.xcrossing.detail == NotifyVirtual ||
		 event.xcrossing.detail == NotifyNonlinearVirtual )
		return;

	// meh, let's leave the last window as the grabbed window for the extra events...
	if ( w )
	{
		uint8_t mods = ( (((event.xcrossing.state & ControlMask) != 0) ? 0x01 : 0) |
						 (((event.xcrossing.state & ShiftMask) != 0) ? 0x02 : 0) );
		w->process_event( event::mouse( _mouse.get(), event_type::MOUSE_LEAVE,
										event.xcrossing.x, event.xcrossing.y, 0, mods ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchFocusIn( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w && _xim )
		XSetICFocus( w->input_context() );
}

////////////////////////////////////////

void dispatcher::dispatchFocusOut( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w && _xim )
		XUnsetICFocus( w->input_context() );
}

////////////////////////////////////////

void dispatcher::dispatchKeymapNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	int min_keycode = 8;
	int max_keycode = 255;
	XDisplayKeycodes( _display.get(), &min_keycode, &max_keycode );
	int ks_per_kc = 1;
	KeySym *ks = XGetKeyboardMapping( _display.get(), min_keycode, max_keycode - min_keycode + 1, &ks_per_kc );

	if ( ks )
	{
		int n = 0;
		for ( int k = min_keycode; k <= max_keycode; ++k )
		{
			auto &ksl = _keycode_to_keysym[k];
			ksl.resize( ks_per_kc );
			for ( int s = 0; s < ks_per_kc; ++s, ++n )
			{
				if ( ks[n] != 0 )
					ksl[s] = ks[n];
			}
		}
		XFree( ks );
	}

	for ( int n = 0; n < 32; ++n )
	{
		char kv = event.xkeymap.key_vector[n];
		if ( kv == 0 )
			continue;
		for ( int i = 0; i < 8; ++i )
		{
			if ( ( kv >> i ) & 0x1 )
			{
//				int k = n * 8 + i;
//				std::cout << "Key " << k << " down: " << int(XK_semicolon) << std::endl;
//				auto &ksl = _keycode_to_keysym[k];
//				for ( auto ks: ksl )
//				{
//					std::cout << "  -> ks " << ks << std::endl;
//				}
			}
		}
	}
}

////////////////////////////////////////

void dispatcher::dispatchExpose( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		if ( event.xexpose.count == 0 )
		{
			w->process_event( event::window( _ext_events.get(),
											 event_type::WINDOW_EXPOSED,
											 event.xexpose.x, event.xexpose.y,
											 event.xexpose.width, event.xexpose.height ) );
		}
		else
		{
//			w->process_event( event::window( _ext_events.get(),
//											 event_type::WINDOW_REGION_EXPOSED,
//											 event.xexpose.x, event.xexpose.y,
//											 event.xexpose.width, event.xexpose.height ) );
			w->add_expose( rect( event.xexpose.x, event.xexpose.y,
								 event.xexpose.width, event.xexpose.height ) );
		}
	}
}

////////////////////////////////////////

void dispatcher::dispatchGraphicsExpose( const std::shared_ptr<window> &w, XEvent &event )
{
	// TODO: this is for XCopyArea, etc, what should we do?
}

////////////////////////////////////////

void dispatcher::dispatchNoExpose( const std::shared_ptr<window> &w, XEvent &event )
{
	// TODO: this is for XCopyArea, etc, what should we do?
}

////////////////////////////////////////

void dispatcher::dispatchVisibilityNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchCreateNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchDestroyNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		if ( _xim )
			XUnsetICFocus( w->input_context() );
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_DESTROYED, 0, 0, 0, 0 ) );
		_windows.erase( w->id() );
	}
}

////////////////////////////////////////

void dispatcher::dispatchUnmapNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		if ( _xim )
			XUnsetICFocus( w->input_context() );
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_HIDDEN, 0, 0, 0, 0 ) );
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_MINIMIZED, 0, 0, 0, 0 ) );
		// add icccm for minimized state storage?
	}
}

////////////////////////////////////////

void dispatcher::dispatchMapNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
	{
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_SHOWN, 0, 0, 0, 0 ) );
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_RESTORED, 0, 0, 0, 0 ) );
	}
}

////////////////////////////////////////

void dispatcher::dispatchMapRequest( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchReparentNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchConfigureNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( w )
		w->process_event( event::window( _ext_events.get(),
										 event_type::WINDOW_MOVE_RESIZE,
										 event.xconfigure.x, event.xconfigure.y,
										 event.xconfigure.width, event.xconfigure.height ) );
}

////////////////////////////////////////

void dispatcher::dispatchConfigureRequest( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchGravityNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchResizeRequest( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchCirculateNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchCirculateRequest( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchPropertyNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( _sel_stack.empty() )
		return;

	SelectionRequestInfo &req = _sel_stack.top();
	if ( req.incr_request && event.xproperty.state == PropertyNewValue && event.xproperty.atom == req.sel )
	{
		Property prop = read_property( req.sel );
		on_scope_exit{ if ( prop.data ) XFree( prop.data ); };

		size_t len = (prop.nitems * prop.format / 8);
		// got our data
		if ( prop.data && len > 0 )
			req.result->insert( req.result->end(), prop.data, prop.data + len );

		if ( len == 0 )
			req.fin->store( true, std::memory_order_relaxed );
		XDeleteProperty( _display.get(), _clipboard_win, req.sel );
	}
}

////////////////////////////////////////

void dispatcher::dispatchSelectionClear( const std::shared_ptr<window> &w, XEvent &event )
{
	std::cout << "clear selection..." << std::endl;
	_sel.clear();
}

////////////////////////////////////////

void dispatcher::dispatchSelectionRequest( const std::shared_ptr<window> &w, XEvent &event )
{
	initSelectionAtoms();

//	Window owner = event.xselectionrequest.owner;
	Atom sel = event.xselectionrequest.selection;
	Atom targ = event.xselectionrequest.target;
	Atom prop = event.xselectionrequest.property;
	Window req = event.xselectionrequest.requestor;
	Time timestamp = event.xselectionrequest.time;
	Display *disp = event.xselectionrequest.display;

	std::cout << "a selection request has arrived..." << std::endl;

	XEvent s;
	s.xselection.type = SelectionNotify;
	s.xselection.requestor = req;
	s.xselection.selection = sel;
	s.xselection.target = targ;
	s.xselection.property = None; // unless changed, this is a refusal
	s.xselection.time = timestamp;

	if ( targ == _sel_targets )
	{
		XChangeProperty( disp, req, prop, XA_ATOM, 32, PropModeReplace,
						 reinterpret_cast<unsigned char *>( _sel_avail_types.data() ),
						 _sel_avail_types.size() );
		s.xselection.property = prop;
	}
	else if ( targ == _sel_timestamp )
	{
		XChangeProperty( disp, req, prop, XA_ATOM, 32, PropModeReplace,
						 reinterpret_cast<unsigned char *>( &_sel_time ),
						 1 );
		s.xselection.property = prop;
	}
	else
	{
		auto x = _sel_type_pool.find( targ );
		if ( x != _sel_type_pool.end() )
		{
			const auto &seldata = _sel.as( x->first );
			// TODO: handle BadAlloc and send w/ INCR?
			XChangeProperty( disp, req, prop, targ, 8, PropModeReplace,
							 seldata.data(), seldata.size() );
		}
	}
	XSendEvent( disp, req, True, 0, &s );
}

////////////////////////////////////////

void dispatcher::dispatchSelectionNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	Atom targ = event.xselection.target;
	std::cout << "selection notify:\n  requestor 0x" << std::hex << event.xselection.requestor << std::dec
			  << "\n  sel atom '" << XGetAtomName( _display.get(), event.xselection.selection )
			  << "'\n  targ atom '" << XGetAtomName( _display.get(), targ )
			  << "'\n  prop atom '" << XGetAtomName( _display.get(), event.xselection.property )
			  << "'" << std::endl;

	if ( _sel_stack.empty() )
		return;

	SelectionRequestInfo &req = _sel_stack.top();
	if ( event.xselection.property == None )
	{
		// unable to convert or no selection (no sel if targ == _sel_targets)
		// but we don't care either way, just clear and finish...
		req.result->clear();
		req.resulttype->clear();
		req.fin->store( true, std::memory_order_relaxed );
		return;
	}

	Property prop = read_property( req.sel );
	on_scope_exit{ if ( prop.data ) XFree( prop.data ); };

	if ( prop.type == _sel_incr )
	{
		std::cout << "Handling large bytes " << prop.format << ", items " << prop.nitems << " expected bytes: " << reinterpret_cast<int *>(prop.data)[0] << std::endl;
		req.incr_request = true;
	}

	if ( targ == _sel_targets && ! req.sent_request )
	{
		if ( fill_requested( req, prop ) )
		{
			std::cout << "requesting type: " << *(req.resulttype) << std::endl;
			req.result->clear();
			XConvertSelection( _display.get(), req.sel, req.requested, req.sel, _clipboard_win, CurrentTime );
		}
	}
	else if ( targ == req.requested )
	{
		size_t len = (prop.nitems * prop.format / 8);
		// got our data
		if ( prop.data && len > 0 )
			req.result->insert( req.result->end(), prop.data, prop.data + len );
		if ( req.incr_request )
		{
			// do we also need to trigger something in propertynotify when the property is re-created?????
			if ( len == 0 )
				req.fin->store( true, std::memory_order_relaxed );
			XDeleteProperty( _display.get(), _clipboard_win, req.sel );
		}
		else
			req.fin->store( true, std::memory_order_relaxed );
	}
}

////////////////////////////////////////

void dispatcher::dispatchColormapNotify( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchClientMessage( const std::shared_ptr<window> &w, XEvent &event )
{
	Atom evatom = static_cast<Atom>( event.xclient.data.l[0] );
	if ( w && ( evatom == _atom_delete_window || evatom == _atom_quit_app ) )
	{
		bool doWinClose = true;
		if ( evatom == _atom_quit_app )
			doWinClose = w->process_event( event::window( _ext_events.get(), event_type::WINDOW_CLOSE_REQUEST, 0, 0, 0, 0 ) );
		else
			w->process_event( event::window( _ext_events.get(),
											 event_type::WINDOW_DESTROYED, 0, 0, 0, 0 ) );

		if ( doWinClose )
		{
			if ( evatom == _atom_quit_app )
				_windows.clear();
			else
				_windows.erase( w->id() );
		}
	}
}

////////////////////////////////////////

void dispatcher::dispatchMappingNotify( const std::shared_ptr<window> &w, XEvent &event )
{
	if ( event.xmapping.request == MappingKeyboard || event.xmapping.request == MappingModifier )
		XRefreshKeyboardMapping( &event.xmapping );
}

////////////////////////////////////////

void dispatcher::dispatchGenericEvent( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchUNKNOWN( const std::shared_ptr<window> &w, XEvent &event )
{
}

////////////////////////////////////////

void dispatcher::dispatchRandRChange( XEvent &event )
{
	std::cout << "updating screen resolutions" << std::endl;
	for ( auto &s: _system->screens() )
		reinterpret_cast<screen *>( s.get() )->update_resolution();
}


////////////////////////////////////////

bool
dispatcher::fill_requested( SelectionRequestInfo &req, const Property &prop )
{
	static std::vector<std::string> knownTypes{
		"UTF8_STRING", "text/plain;charset=utf-8", "STRING", "C_STRING", "text/plain",
			"TEXT", "COMPOUND_TEXT" };

	req.sent_request = true;
	req.requested = None;

	Atom *alist = reinterpret_cast<Atom *>( prop.data );

	if ( ( prop.type != XA_ATOM && prop.type != _sel_targets ) || prop.format != 32 )
	{
		if ( req.reqTypes )
		{
			auto &reqTypes = *(req.reqTypes);
			req.resulttype->assign( "text/plain" );
			if ( reqTypes.empty() )
				req.requested = XA_STRING;
			else if ( std::find( reqTypes.begin(), reqTypes.end(), *(req.resulttype) ) != reqTypes.end() )
				req.requested = XA_STRING;
		}
		else if ( req.reqTypeFunc )
		{
			std::vector<std::string> tmp{ 1, "text/plain" };
			(*req.resulttype) = (*req.reqTypeFunc)( tmp );
			req.requested = ( req.resulttype->empty() ) ? None : XA_STRING;
		}
	}
	else if ( req.reqTypes )
	{
		auto &reqTypes = *(req.reqTypes);

		size_t bIdx = size_t(-1);
		for ( int i = 0; i < prop.nitems; ++i )
		{
			char *anameptr = XGetAtomName( _display.get(), alist[i] );
			if ( ! anameptr )
				continue;

			if ( reqTypes.empty() )
			{
				for ( size_t k = 0; k != knownTypes.size(); ++k )
				{
					if ( knownTypes[k].compare( anameptr ) == 0 )
					{
						if ( k < bIdx )
						{
							req.resulttype->assign( anameptr );
							req.requested = alist[i];
							bIdx = k;
						}
						break;
					}
				}
			}
			else
			{
				for ( size_t r = 0; r != reqTypes.size(); ++r )
				{
					if ( reqTypes[r].compare( anameptr ) == 0 )
					{
						if ( r < bIdx )
						{
							req.requested = alist[i];
							bIdx = r;
						}
						break;
					}
				}
			}
		}
		if ( bIdx != size_t(-1) && ! reqTypes.empty() )
			req.resulttype->assign( reqTypes[bIdx] );
	}
	else if ( req.reqTypeFunc )
	{
		std::vector<std::string> names;
		// stash this off to avoid roundtrips to the server
		std::map<std::string, Atom> namemap;
		names.reserve( prop.nitems );
		for ( int i = 0; i < prop.nitems; ++i )
		{
			char *anameptr = XGetAtomName( _display.get(), alist[i] );
			if ( ! anameptr )
				continue;

			names.emplace_back( anameptr );
			namemap[names.back()] = alist[i];
		}

		(*req.resulttype) = (*req.reqTypeFunc)( names );
		if ( ! req.resulttype->empty() )
		{
			auto i = namemap.find( (*req.resulttype) );
			if ( i != namemap.end() )
			{
				req.requested = i->second;
			}
		}
	}

	if ( req.requested == None )
	{
		req.result->clear();
		req.resulttype->clear();
		req.fin->store( true, std::memory_order_relaxed );
		return false;
	}

	return true;
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform

