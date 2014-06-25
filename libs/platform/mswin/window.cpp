
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
	HINSTANCE hInstance = GetModuleHandle(NULL);
	_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"WindowClass",
		L"Title",
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
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
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

	// Create an OpenGL 2.1 context for our device context
	HGLRC tempOpenGLContext = wglCreateContext( _hdc );
	wglMakeCurrent( _hdc, tempOpenGLContext );

	glewExperimental = true;
	GLenum error = glewInit();
	if ( error != GLEW_OK )
		throw std::runtime_error( "glew init failed" );

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if ( wglewIsSupported( "WGL_ARB_create_context" ) == 1 )
	{
		_hrc = wglCreateContextAttribsARB( _hdc, NULL, attributes );
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( tempOpenGLContext );
		wglMakeCurrent( _hdc, _hrc );
	}
	else
		throw std::runtime_error( "opengl 4 not supported" );

	int glVersion[2];
	glGetIntegerv( GL_MAJOR_VERSION, &glVersion[0] );
	glGetIntegerv( GL_MINOR_VERSION, &glVersion[1] );

	_canvas = std::make_shared<draw::canvas>();

	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
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
	if ( !wglMakeCurrent( _hdc, _hrc ) )
		throw std::runtime_error( "couldn't make context current" );
	return gl::context();
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	if ( !wglMakeCurrent( _hdc, _hrc ) )
		throw std::runtime_error( "couldn't make context current" );
	return _canvas;
}

////////////////////////////////////////

void window::expose_event( void )
{
//	PAINTSTRUCT ps;
//	BeginPaint( _hwnd, &ps );
	exposed();
//	EndPaint( _hwnd, &ps );
	SwapBuffers( _hdc );
}

////////////////////////////////////////

} }

