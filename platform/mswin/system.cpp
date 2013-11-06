
#include "system.h"
#include "screen.h"
#include "window.h"
#include "timer.h"
#include "font_manager.h"
#include "dispatcher.h"

#include <core/contract.h>
#include <stdexcept>

namespace {

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

}



namespace mswin
{

////////////////////////////////////////

system::system( void )
	: platform::system( "mswin", "Microsoft Windows" )
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

	if ( !RegisterClassEx( &wc ) )
		throw std::runtime_error( "window registration failed" );

	_keyboard = std::make_shared<keyboard>();
	_mouse = std::make_shared<mouse>();
	_font_manager = std::make_shared<font_manager>();
	_dispatcher = std::make_shared<dispatcher>( _keyboard, _mouse );
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>();
	return ret;
}

////////////////////////////////////////

std::shared_ptr<platform::timer> system::new_timer( void )
{
	return std::make_shared<timer>();
}

////////////////////////////////////////

std::shared_ptr<platform::dispatcher> system::get_dispatcher( void )
{
	return _dispatcher;
}

////////////////////////////////////////

std::shared_ptr<platform::keyboard> system::get_keyboard( void )
{
	return _keyboard;
}

////////////////////////////////////////

std::shared_ptr<platform::mouse> system::get_mouse( void )
{
	return _mouse;
}

////////////////////////////////////////

std::shared_ptr<platform::font_manager> system::get_font_manager( void )
{
	return _font_manager;
}

////////////////////////////////////////

}
