//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <mutex>
#include "opengl.h"
#include <base/contract.h>

namespace
{

HGLRC WINAPI (*wglCreateContextAttribsARB)( HDC, HGLRC, const int * ) = nullptr;

// prescribed method to enable nvidia GPU on laptops w/ dual GPU
//extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

HMODULE libgl = NULL;
HWND coreWindow = NULL;
HGLRC coreContext = NULL;
std::once_flag opengl_init_flag;

HGLRC doCreate( HDC dc, HGLRC parContext )
{
	bool docreate = dc != nullptr;

	if ( dc == nullptr )
	{
		if ( coreWindow == nullptr )
		{
			coreWindow = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				"WindowClass",
				"Title",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL, NULL, GetModuleHandle(NULL), NULL );
			if ( coreWindow == nullptr )
				throw_runtime( "Unable to create window" );
		}

		dc = GetDC( coreWindow );
	}

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
	int nPixelFormat = ChoosePixelFormat( dc, &pfd );
	if ( nPixelFormat == 0 )
		throw std::runtime_error( "pixel format failed" );

   	// Try and set the pixel format based on our PFD
	bool bResult = SetPixelFormat( dc, nPixelFormat, &pfd );
	if ( !bResult )
		throw_runtime( "set pixel format failed" );

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	if ( ! coreContext )
	{
		HGLRC oldCtxt = wglGetCurrentContext();

		HDC tmpdc = GetDC( coreWindow );

		HGLRC tempOpenGLContext = wglCreateContext( tmpdc );
		wglMakeCurrent( tmpdc, tempOpenGLContext );

		wglCreateContextAttribsARB = reinterpret_cast<decltype(wglCreateContextAttribsARB)>( wglGetProcAddress("wglCreateContextAttribsARB") );
		if ( ! wglCreateContextAttribsARB )
		{
			wglMakeCurrent( tmpdc, oldCtxt );
			wglDeleteContext( tempOpenGLContext );
			DestroyWindow( coreWindow );
			coreWindow = nullptr;
			throw_runtime( "Unable to find wglCreateContextAttribsARB" );
		}

		coreContext = wglCreateContextAttribsARB( tmpdc, parContext, attributes );
		if ( coreContext == nullptr )
		{
			wglMakeCurrent( dc, oldCtxt );
			wglDeleteContext( tempOpenGLContext );
			DestroyWindow( coreWindow );
			coreWindow = nullptr;
			throw_lasterror( "unable to create core OpenGL context" );
		}

		wglMakeCurrent( tmpdc, coreContext );
		wglDeleteContext( tempOpenGLContext );
		std::cout << "initializing OpenGL function pointers..." << std::endl;
		bool ok = gl3wInit2( platform::mswin::queryGL );
		wglMakeCurrent( tmpdc, oldCtxt );
		if ( ! ok )
			throw_runtime( "Unable to initialize OpenGL" );
		std::cout << "finished initializing OpenGL..." << std::endl;
	}

	if ( docreate )
	{
		HGLRC oldCtxt = wglGetCurrentContext();
		wglMakeCurrent( dc, coreContext );
		HGLRC ret = wglCreateContextAttribsARB( dc, parContext, attributes );
		wglMakeCurrent( dc, oldCtxt );
		if ( ret == nullptr )
			throw_lasterror( "unable to create OpenGL context" );
		return ret;
	}
	return nullptr;
}

void shutdown_libgl( void )
{
	wglMakeCurrent( NULL, NULL );
	if ( coreContext )
	{
		wglDeleteContext( coreContext );
		coreContext = nullptr;
	}
	if ( coreWindow )
	{
		DestroyWindow( coreWindow );
		coreWindow = nullptr;
	}
	if ( libgl )
		FreeLibrary( libgl );
	libgl = NULL;
}

void init_libgl( void )
{
	libgl = LoadLibraryA( "opengl32.dll" );
	atexit( shutdown_libgl );

	doCreate( NULL, NULL );
}

} // anonymous namespace


namespace platform
{
namespace mswin
{

HGLRC
createOGLContext( HDC dc )
{
	std::call_once( opengl_init_flag, [](){ init_libgl(); } );

	return doCreate( dc, coreContext );
}

platform::system::opengl_func_ptr
queryGL( const char *f )
{
//	std::call_once( opengl_init_flag, [](){ init_libgl(); } );

	platform::system::opengl_func_ptr res;

	HGLRC oldCtxt = wglGetCurrentContext();
	if ( oldCtxt != coreContext )
		wglMakeCurrent( GetDC( coreWindow ), coreContext );
	res = (platform::system::opengl_func_ptr) wglGetProcAddress( f );
	if ( oldCtxt != coreContext )
		wglMakeCurrent( GetDC( coreWindow ), oldCtxt );

	if ( ! res && libgl )
		res = (platform::system::opengl_func_ptr) GetProcAddress( libgl, f );

	return res;
}

} // namespace mswin
} // namespace platform



