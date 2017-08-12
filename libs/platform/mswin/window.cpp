//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"

#include <iostream>
#include <stdexcept>

#include <windows.h>
#include <base/win32_system_error.h>

#include <gl/opengl.h>

namespace
{
	HGLRC WINAPI (*wglCreateContextAttribsARB)( HDC, HGLRC, const int * ) = nullptr;
}

// prescribed method to enable nvidia GPU on laptops w/ dual GPU
//extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

namespace platform { namespace mswin
{

////////////////////////////////////////

window::window( void )
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"WindowClass",
		"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );

	if ( _hwnd == nullptr )
		throw std::runtime_error( "window creation failed" );

	_hdc = GetDC( _hwnd ); // Get the device context for our window

	// Create a new PIXELFORMATDESCRIPTOR (PFD)
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(PIXELFORMATDESCRIPTOR) );
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;//32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

   	// Check if our PFD is valid and get a pixel format back
	int nPixelFormat = ChoosePixelFormat( _hdc, &pfd );
	if ( nPixelFormat == 0 )
		throw std::runtime_error( "pixel format failed" );

   	// Try and set the pixel format based on our PFD
	bool bResult = SetPixelFormat( _hdc, nPixelFormat, &pfd );
	if ( !bResult )
		throw std::runtime_error( "set pixel format failed" );

	HGLRC tempOpenGLContext = wglCreateContext( _hdc );
	wglMakeCurrent( _hdc, tempOpenGLContext );

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	wglCreateContextAttribsARB = reinterpret_cast<decltype(wglCreateContextAttribsARB)>( wglGetProcAddress("wglCreateContextAttribsARB") );
	if ( wglCreateContextAttribsARB == nullptr )
		throw std::runtime_error( "wgl create context missing" );

	_hrc = wglCreateContextAttribsARB( _hdc, NULL, attributes );
	if ( _hrc == nullptr )
		throw_win32_error( "unable to create OpenGL context" );

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( tempOpenGLContext );
	wglMakeCurrent( _hdc, _hrc );

	update_position();
	//wglCreateLayerContext

//	int glVersion[2];
//	glGetIntegerv( GL_MAJOR_VERSION, &glVersion[0] );
//	glGetIntegerv( GL_MINOR_VERSION, &glVersion[1] );
//	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
}

////////////////////////////////////////

window::~window( void )
{
	if ( _hwnd )
		DestroyWindow( _hwnd );
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

void window::move( double x, double y )
{
}

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
	if ( rect.left == rect.top &&
		 rect.left == rect.right &&
		 rect.left == rect.bottom )
		RedrawWindow( _hwnd, NULL, NULL, RDW_INTERNALPAINT );
	else
		RedrawWindow( _hwnd, &rect, NULL, RDW_INTERNALPAINT );
//	InvalidateRect( _hwnd, &rect, FALSE );
}

////////////////////////////////////////

void window::acquire( void )
{
	if ( !wglMakeCurrent( _hdc, _hrc ) )
		throw std::runtime_error( "couldn't make context current" );
}

////////////////////////////////////////

void window::release( void )
{
	wglMakeCurrent( NULL, NULL );
}

////////////////////////////////////////

void window::update_position( void )
{
	RECT pos;
	GetWindowRect( _hwnd, &pos );
	// TODO: do we have to handle the resize handle width???
	_last_x = pos.left;
	_last_y = pos.top;
	_last_w = pos.right - pos.left + 1;
	_last_h = pos.bottom - pos.top + 1;
	std::cout << "window resized to: " << _last_w << " x " << _last_h << " (" << pos.left << ", " << pos.top << " " << pos.right << ", " << pos.bottom << ")" << std::endl;
}


////////////////////////////////////////

void window::expose_event( void )
{
	acquire();
//	PAINTSTRUCT ps;
//	BeginPaint( _hwnd, &ps );
	exposed();
//	EndPaint( _hwnd, &ps );
//	wglSwapLayerBuffers( _hdc, WGL_SWAP_MAIN_PLANE );
	SwapBuffers( _hdc );
	release();
}

////////////////////////////////////////

} }

