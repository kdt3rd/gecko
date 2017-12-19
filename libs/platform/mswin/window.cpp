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
#include <base/contract.h>

#include "opengl.h"

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
		throw_runtime( "window creation failed" );

	_hdc = GetDC( _hwnd );
	_hrc = createOGLContext( _hdc );

	wglMakeCurrent( _hdc, _hrc );

	if ( !gl3wIsSupported( 3, 3 ) )
		throw_runtime( "OpenGL 3.3 not supported" );

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
	if ( _hrc )
	{
		HGLRC oldCtxt = wglGetCurrentContext();
		if ( oldCtxt == _hrc )
			wglMakeCurrent( _hdc, NULL );
		wglDeleteContext( _hrc );
	}

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

void
window::fullscreen( bool fs )
{
	std::cout << "implement window::fullscreen" << std::endl;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::move( coord_type x, coord_type y )
{
}

////////////////////////////////////////

void window::resize( coord_type w, coord_type h )
{
}

////////////////////////////////////////

void window::set_minimum_size( coord_type w, coord_type h )
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

void window::invalidate( const rect &r )
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

void
window::make_current( const std::shared_ptr<cursor> &c )
{
	std::cout << "NYI: make_current( cursor )" << std::endl;
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

void
window::move_event( coord_type x, coord_type y )
{
	_last_x = x;
	_last_y = y;
}

////////////////////////////////////////

void
window::resize_event( coord_type w, coord_type h )
{
	_last_w = w;
	_last_h = h;
}

////////////////////////////////////////

} }

