//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "dispatcher.h"

#include <platform/scancode.h>
#include <platform/event_queue.h>

#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include <windows.h>
#include <windowsx.h>

namespace {

platform::mswin::dispatcher *the_glob_disp = nullptr;

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static RECT sizemovergn;
	static bool insizemove = false;

	LRESULT retval = false;

	if ( ! the_glob_disp )
		return DefWindowProc( hwnd, msg, wParam, lParam );

    switch(msg)
    {
		case WM_DISPLAYCHANGE:
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			return false;

		case WM_PAINT:
		{
			RECT rgn;
			if ( GetUpdateRect( hwnd, &rgn, false ) )
			{
				wParam = MAKEWPARAM( (WORD)rgn.left, (WORD)rgn.top );
				lParam = MAKELPARAM( (WORD)rgn.right, (WORD)rgn.bottom );
				if ( ! insizemove )
					the_glob_disp->add_event( hwnd, msg, wParam, lParam );

				retval = DefWindowProc( hwnd, msg, wParam, lParam );
			}
			break;
		}

		case WM_SIZING:
		{
			LPRECT lprc = (LPRECT)lParam;
			sizemovergn = *lprc;

			if ( insizemove &&
				 sizemovergn.left != sizemovergn.right &&
				 sizemovergn.top != sizemovergn.bottom )
				the_glob_disp->deliver_event( hwnd, msg, wParam, lParam );

			retval = true;
			break;
		}

		case WM_ENTERSIZEMOVE:
			insizemove = true;
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			break;

		case WM_EXITSIZEMOVE:
			insizemove = false;
			retval = DefWindowProc( hwnd, msg, wParam, lParam );

			// send an event to redraw since we were ignoring them during
			// size/move
			the_glob_disp->add_event( hwnd, WM_ERASEBKGND, wParam, lParam );
			break;

		case WM_WINDOWPOSCHANGED:
		{
			if ( ! insizemove )
				the_glob_disp->deliver_event( hwnd, msg, wParam, lParam );
//			else
//			{
//				LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
//				sizemovergn.left = pwp->x;
//				sizemovergn.top = pwp->y;
//				sizemovergn.right = pwp->cx;
//				sizemovergn.bottom = pwp->cy;
//			}
			retval = 0;
			break;
		}

		case WM_SYSCHAR:
		{
			TCHAR ccode = (TCHAR) wParam;
			if ( ' ' == ccode )
				retval = DefWindowProc( hwnd, msg, wParam, lParam );
			else
				retval = true;
			break;
		}
			
		case WM_SYSDEADCHAR:
			break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			unsigned short key = ( HIWORD( lParam ) & 0x0FFF );
			if ( key == 0x39 || key == 0x3E )
				retval = DefWindowProc( hwnd, msg, wParam, lParam );
			else
				the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			break;
		}

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			break;

		case WM_SETCURSOR:
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			break;

		case WM_ERASEBKGND:
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			// cheat and say we've erased even though we haven't yet
			return true;

		case WM_CLEAR:
//			if ( OpenClipboard( nullptr ) )
//				EmptyClipboard();
//			CloseClipboard();
		case WM_COPY:
		case WM_CUT:
		case WM_PASTE:
			return true;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			return false;

		case WM_ACTIVATE:
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			break;

		case WM_CLOSE:
			// we will let the system call the DestroyWindow as
			// appropriate...
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			retval = 0;
			break;
		case WM_DESTROY:
			// TODO: handle clipboard chain (see windows docs)
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			break;

		case WM_MOVE:
		case WM_SIZE:
		case WM_QUIT:
		case WM_CREATE:
		case WM_PALETTECHANGED:
		case WM_QUERYNEWPALETTE:
			the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			break;

		case WM_NCHITTEST:
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			if ( HTCLIENT == retval )
				the_glob_disp->add_event( hwnd, msg, wParam, lParam );
			break;

		default:
			retval = DefWindowProc( hwnd, msg, wParam, lParam );
			break;
    }

	return retval;
}

}

////////////////////////////////////////

namespace platform { namespace mswin
{

////////////////////////////////////////

dispatcher::dispatcher( ::platform::system *sys, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _system( sys ), _keyboard( k ), _mouse( m )
{
	precondition( the_glob_disp == nullptr, "expect only a single dispatcher" );
	the_glob_disp = this;
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle( NULL );
    wc.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = "WindowMenu";
    wc.lpszClassName = "WindowClass";
    wc.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );

	_normal_window_class = RegisterClassEx( &wc );
	if ( _normal_window_class == 0 )
		throw_lasterror( "window class registration failed" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
	the_glob_disp = nullptr;
	if ( _normal_window_class != 0 )
		UnregisterClass( "WindowClass", GetModuleHandle( NULL ) );
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;

	MSG Msg;
	DWORD millis = 0;
	while ( ! _exit_requested.load( std::memory_order_relaxed ) )
	{
		while ( ! _events.empty() )
		{
			dispatch_evt( _events.front() );
			_events.pop_front();
		}

		if ( _windows.empty() )
			break;

		// TODO: this only works for windows created in this thread
		millis = INFINITE;
		waitable::time_point curt = waitable::clock::now();
		for ( auto &w: _waitables )
		{
			waitable::duration when;
			if ( w->poll_timeout( when, curt ) )
			{
				if ( when < waitable::duration::zero() )
					w->emit( curt );
				else
				{
					std::chrono::milliseconds msecs = std::chrono::duration_cast<std::chrono::milliseconds>( when );
					if ( millis == INFINITE )
						millis = DWORD( msecs.count() );
					else
						millis = std::min( millis, DWORD( msecs.count() ) );
				}
			}
		}

		// TODO: convert to MsgWaitForMultipleObjectsEx
		if ( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &Msg );
			DispatchMessage( &Msg );
		}
		else
		{
			DWORD w = MsgWaitForMultipleObjectsEx( 0, NULL, millis, QS_ALLINPUT, MWMO_ALERTABLE|MWMO_INPUTAVAILABLE );
			if ( w == WAIT_FAILED )
			{
				std::cout << "message wait failed..." << std::endl;
			}
		}

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

void dispatcher::add_window( const std::shared_ptr<window> &win )
{
	_windows[win->id()] = win;
}

////////////////////////////////////////

void
dispatcher::remove_window( const std::shared_ptr<window> &win )
{
	_windows.erase( win->id() );
}

////////////////////////////////////////

bool dispatcher::deliver_event( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	evt e{ hw, msg, wp, lp };
	return dispatch_evt( e );
}

////////////////////////////////////////

void dispatcher::add_event( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	_events.emplace_back( hw, msg, wp, lp );
}

////////////////////////////////////////

bool
dispatcher::dispatch_evt( const evt &e )
{
	HWND hwnd = e.hwnd;
	UINT msg = e.msg;
	WPARAM wp = e.wp;
	WPARAM lp = e.lp;
	bool ret = true;

	auto i = _windows.find( hwnd );
	if ( i == _windows.end() )
		return ret;
	const std::shared_ptr<window> &w = i->second;

    switch(msg)
    {
		case WM_DISPLAYCHANGE:
			std::cout << "HANDLE screen resolution change" << std::endl;
			break;

		case WM_CLOSE:
			if ( w->process_event( event::window( _system, _ext_events.get(), event_type::WINDOW_CLOSE_REQUEST, coord_type(0), coord_type(0), coord_type(0), coord_type(0) ) ) )
			{
				_windows.erase( w->id() );
				return true;
			}
			return false;

        case WM_DESTROY:
			_windows.erase( w->id() );
			return true;

		case WM_PAINT:
		{
			RECT rgn;
			rgn.left = LOWORD( wp );
			rgn.top = HIWORD( wp );
			rgn.right = LOWORD( lp );
			rgn.bottom = HIWORD( lp );
			w->process_event(
				event::window( _system, _ext_events.get(),
							   event_type::WINDOW_EXPOSED,
							   coord_type(rgn.left),
							   coord_type(rgn.top),
							   coord_type(rgn.right - rgn.left + 1),
							   coord_type(rgn.bottom - rgn.top + 1) ) );
			break;
		}

		case WM_SIZING:
		{
			LPRECT lprc = (LPRECT)lp;
			// TODO: fit to window min / max
			// lprc->right = min / max / cur;
			// lprc->bottom = min / max / cur;

			w->process_event(
				event::window( _system, _ext_events.get(),
							   event_type::WINDOW_MOVE_RESIZE,
							   coord_type(lprc->left),
							   coord_type(lprc->top),
							   coord_type(lprc->right - lprc->left + 1),
							   coord_type(lprc->bottom - lprc->top + 1 )) );
			return true;
		}

		case WM_ENTERSIZEMOVE:
		case WM_EXITSIZEMOVE:
			break;

		case WM_WINDOWPOSCHANGED:
		{
			LPWINDOWPOS pwp = (LPWINDOWPOS)lp;
			w->process_event(
				event::window( _system, _ext_events.get(),
							   event_type::WINDOW_MOVE_RESIZE,
							   coord_type(pwp->x),
							   coord_type(pwp->y),
							   coord_type(pwp->cx),
							   coord_type(pwp->cy) ) );
			return true;
		}

		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			std::cout << " handle keydown: " << HIWORD( lp ) << std::endl;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			std::cout << " handle keyup: " << HIWORD( lp ) << std::endl;
			break;

		case WM_LBUTTONDOWN:
			send_mouse_evt( w, event_type::MOUSE_DOWN, 1, wp, lp );
			break;
		case WM_MBUTTONDOWN:
			send_mouse_evt( w, event_type::MOUSE_DOWN, 2, wp, lp );
			break;
		case WM_RBUTTONDOWN:
			send_mouse_evt( w, event_type::MOUSE_DOWN, 3, wp, lp );
			break;

		case WM_LBUTTONUP:
			send_mouse_evt( w, event_type::MOUSE_UP, 1, wp, lp );
			break;
		case WM_MBUTTONUP:
			send_mouse_evt( w, event_type::MOUSE_UP, 2, wp, lp );
			break;
		case WM_RBUTTONUP:
			send_mouse_evt( w, event_type::MOUSE_UP, 3, wp, lp );
			break;
		case WM_MOUSEWHEEL:
		{
			POINT p;
			p.x = GET_X_LPARAM( lp );
			p.y = GET_Y_LPARAM( lp );
			//ScreenToClient( w->id(), &p );
			int amt = HIWORD( wp ) / WHEEL_DELTA;
			uint8_t mods = extract_mods( LOWORD( wp ) );
			w->process_event(
				event::hid( _system, _mouse.get(), event_type::MOUSE_WHEEL,
							p.x, p.y, 4, amt, mods ) );
			break;
		}
		case WM_MOUSEMOVE:
			send_mouse_evt( w, event_type::MOUSE_MOVE, 0, wp, lp );
			break;

		case WM_SETCURSOR:
			break;

		case WM_ERASEBKGND:
			w->process_event(
				event::window( _system, _ext_events.get(),
							   event_type::WINDOW_EXPOSED,
							   0, 0, 0, 0 ) );
			break;

		case WM_CLEAR:
//			if ( OpenClipboard( nullptr ) )
//				EmptyClipboard();
//			CloseClipboard();
		case WM_COPY:
		case WM_CUT:
		case WM_PASTE:
			break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
		case WM_ACTIVATE:

		case WM_MOVE:
		case WM_SIZE:
		case WM_QUIT:
		case WM_CREATE:
		case WM_PALETTECHANGED:
		case WM_QUERYNEWPALETTE:
			break;
		case WM_NCHITTEST:
		default:
			break;
    }
	return ret;
}

////////////////////////////////////////

void dispatcher::send_mouse_evt( const std::shared_ptr<window> &w,
								 event_type e, int button, WPARAM wp, LPARAM lp )
{
	POINT p;
	p.x = GET_X_LPARAM( lp );
	p.y = GET_Y_LPARAM( lp );
	//ScreenToClient( w->id(), &p );

	uint8_t mods = extract_mods( LOWORD( wp ) );

	w->process_event(
		event::mouse( _system, _mouse.get(), e, p.x, p.y, button, mods ) );
}

////////////////////////////////////////

uint8_t dispatcher::extract_mods( WORD v )
{
	uint8_t mods = 0;
	if ( v & MK_CONTROL )
		mods |= modifier::LEFT_CTRL;
	if ( v & MK_SHIFT )
		mods |= modifier::LEFT_SHIFT;
	return mods;
}

////////////////////////////////////////

} // namespace mswin
} // namespace platform

