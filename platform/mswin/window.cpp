
#include "window.h"

#include <iostream>
#include <stdexcept>
#include <cairo-win32.h>

#include <core/pointer.h>
#include <core/contract.h>
#include <windows.h>

namespace mswin
{

////////////////////////////////////////

window::window( void )
{
	_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"WindowClass",
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GetModuleHandle(NULL), NULL );

	if ( _hwnd == nullptr )
		throw std::runtime_error( "window creation failed" );

	std::cout << "CREATED WINDOW: " << _hwnd << std::endl;
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
}

////////////////////////////////////////

/*
void window::lower( void )
{
	const static uint32_t values[] = { XCB_STACK_MODE_BELOW };
	xcb_configure_window( _connection, _win, XCB_CONFIG_WINDOW_STACK_MODE, values);
}
*/

////////////////////////////////////////

void window::show( void )
{
	std::cout << "Showing window!" << std::endl;
	ShowWindow( _hwnd, SW_SHOW );
	UpdateWindow( _hwnd );
}

////////////////////////////////////////

void window::hide( void )
{
}

////////////////////////////////////////

bool window::is_visible( void )
{
	// TODO fix this
	return true;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::resize( double w, double h )
{
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	if ( !_canvas )
	{
		_canvas = std::make_shared<cairo::canvas>();

		HDC hdc = GetDC( _hwnd );
		cairo_surface_t *surf = cairo_win32_surface_create( hdc );
		_canvas->set_surface( surf );
	}

	return _canvas;
}

////////////////////////////////////////

void window::exposed( void )
{
	PAINTSTRUCT ps;
	BeginPaint( _hwnd, &ps );
	platform::window::exposed();
	EndPaint( _hwnd, &ps );
}

////////////////////////////////////////

}

