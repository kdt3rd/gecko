//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "dispatcher.h"

#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include <windows.h>

namespace {

std::map<HWND,std::weak_ptr<platform::mswin::window>> windows;

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	std::shared_ptr<platform::mswin::window> win;
	auto i = windows.find( hwnd );
	if ( i != windows.end() )
		win = i->second.lock();

	if ( !win )
		return DefWindowProc( hwnd, msg, wParam, lParam );

    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow( hwnd );
			win->closed();
			break;

        case WM_DESTROY:
            PostQuitMessage(0);
			break;

		case WM_PAINT:
			win->expose_event();
			return DefWindowProc( hwnd, msg, wParam, lParam );

		case WM_EXITSIZEMOVE:
		{
			LRESULT r = DefWindowProc( hwnd, msg, wParam, lParam );
			win->update_position();
			return r;
		}

		default:
			break;
    }
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

}

////////////////////////////////////////

namespace platform { namespace mswin
{

////////////////////////////////////////

dispatcher::dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _keyboard( k ), _mouse( m )
{
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
	if ( _normal_window_class != 0 )
		UnregisterClass( "WindowClass", GetModuleHandle( NULL ) );
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;

	MSG Msg;
	DWORD millis = 0;
	while ( true )
	{
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
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &win )
{
	windows[win->id()] = win;
}

////////////////////////////////////////

void
dispatcher::remove_window( const std::shared_ptr<window> &win )
{
	windows.erase( win->id() );
}

////////////////////////////////////////

} }

