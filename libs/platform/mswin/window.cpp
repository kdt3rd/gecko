
#include "window.h"

#include <iostream>
#include <stdexcept>

#include <windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>

namespace platform { namespace mswin
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

void window::lower( void )
{
}

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

void window::set_popup( void )
{
}

////////////////////////////////////////

void window::invalidate( const base::rect &r )
{
	RECT rect = { LONG( std::floor( r.x1() ) ), LONG( std::floor( r.y1() ) ), LONG( std::ceil( r.x2() ) ), LONG( std::ceil( r.y2() ) ) };
	InvalidateRect( _hwnd, &rect, FALSE );
}

////////////////////////////////////////

gl::context window::context( void )
{
//	glXMakeCurrent( _display.get(), _win, _glc );
	return gl::context();
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	return _canvas;
}

////////////////////////////////////////

/*
void window::exposed( void )
{
	PAINTSTRUCT ps;
	BeginPaint( _hwnd, &ps );
	platform::window::exposed();
	EndPaint( _hwnd, &ps );
}
*/

////////////////////////////////////////

} }

