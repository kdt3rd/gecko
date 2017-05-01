//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include "dispatcher.h"
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
			return 0;

        default:
            return DefWindowProc( hwnd, msg, wParam, lParam );
    }
    return 0;
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
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle( NULL );
    wc.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WindowClass";
    wc.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );

	ATOM ret = RegisterClassEx( &wc );
	if ( !ret )
		throw std::runtime_error( "window registration failed" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;

	MSG Msg;

	while( GetMessage(&Msg, NULL, 0, 0) > 0 )
	{
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
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

} }

