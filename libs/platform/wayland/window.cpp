//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"

#include <base/compiler_support.h>
#include <base/contract.h>
#include <base/pointer.h>
#include <base/scope_guard.h>

#include <gl/check.h>
#include <gl/opengl.h>

#include <string.h>
#include <iostream>
#include <stdexcept>

namespace {


static void shell_ping( void *data, struct wl_shell_surface *surf, uint32_t serial )
{
	wl_shell_surface_pong( surf, serial );
}

static void shell_configure( void *data,
							 struct wl_shell_surface *surf,
							 uint32_t edges,
							 int32_t width, int32_t height )
{
	platform::wayland::window *win = reinterpret_cast<platform::wayland::window *>( data );
	win->resize_event( platform::coord_type( width ), platform::coord_type( height ) );
}

static void shell_popup_done( void *data,
							  struct wl_shell_surface *surf )
{
}

static struct wl_shell_surface_listener shell_listener = {
	&shell_ping,
	&shell_configure,
	&shell_popup_done
};

}

namespace platform
{
namespace wayland
{

////////////////////////////////////////

window::window( EGLDisplay disp, struct wl_compositor *comp, struct wl_shell *shell )
	: _disp( disp )
{
	_last_w = 512; _last_h = 512;

	EGLint attributes[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_CONFORMANT, EGL_OPENGL_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_NONE, EGL_NONE
	};
	EGLint ctxtattribs[] = {
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 3,
//		EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
//		EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_TRUE,
		EGL_NONE, EGL_NONE
		};
	EGLConfig config;
	EGLint num_config = 0;

	if ( ! eglChooseConfig( disp, attributes, &config, 1, &num_config ) )
		throw std::runtime_error( "unable to find valid egl context config" );
	if ( 0 == num_config )
		throw std::runtime_error( "unable to find valid egl context config" );

	TODO("Implement shared context - create one in system, pass it here")
	_egl_context = eglCreateContext( disp, config, EGL_NO_CONTEXT, ctxtattribs );
	if ( ! _egl_context )
		throw std::runtime_error( "Unable to create egl context" );

	TODO("add more error checks")
	_surface = wl_compositor_create_surface( comp );
	_shell_surf = wl_shell_get_shell_surface( shell, _surface );

	wl_shell_surface_add_listener( _shell_surf, &shell_listener, this );
	wl_shell_surface_set_toplevel( _shell_surf );
	_egl_win = wl_egl_window_create( _surface, _last_w, _last_h );
	_egl_surface = eglCreateWindowSurface( disp, config, (EGLNativeWindowType)_egl_win, NULL );
	eglMakeCurrent( disp, _egl_surface, _egl_surface, _egl_context );

	std::cout << "OpenGL:\n\tvendor " << glGetString( GL_VENDOR )
			  << "\n\trenderer " << glGetString( GL_RENDERER )
			  << "\n\tversion " << glGetString( GL_VERSION )
			  << "\n\tshader language " << glGetString( GL_SHADING_LANGUAGE_VERSION )
			  << "\n\n" << std::endl;

	if ( !gl3wIsSupported( 3, 3 ) )
		throw std::runtime_error( "opengl 3.3 not supported" );

	glClearColor( 0.15, 0.15, 0.15, 0.15 );
	glClear( GL_COLOR_BUFFER_BIT );
	eglSwapBuffers( disp, _egl_surface );

}

////////////////////////////////////////

window::~window( void )
{
	eglDestroySurface( _disp, _egl_surface);
	wl_egl_window_destroy( _egl_win );
	wl_shell_surface_destroy( _shell_surf );
	wl_surface_destroy( _surface );
	eglDestroyContext( _disp, _egl_context );
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

void window::set_popup( void )
{
}

////////////////////////////////////////

void window::show( void )
{
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

void window::set_minimum_size( coord_type /*w*/, coord_type /*h*/ )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
	if ( !_invalid )
	{
		// TODO
		_invalid = true;
		expose_event( r.x(), r.y(), r.width(), r.height() );
	}
}

////////////////////////////////////////

void window::acquire( void )
{
	eglMakeCurrent( _disp, _egl_surface, _egl_surface, _egl_context );
}

////////////////////////////////////////

void window::release( void )
{
	eglMakeCurrent( _disp, NULL, NULL, NULL );
}

////////////////////////////////////////

void window::move_event( coord_type x, coord_type y )
{
	int16_t tx = static_cast<int16_t>( x );
	int16_t ty = static_cast<int16_t>( y );
	if ( _last_x != tx || _last_y != ty )
	{
		_last_x = tx;
		_last_y = ty;
		if ( moved )
			moved( x, y );
	}
}

////////////////////////////////////////

void window::resize_event( coord_type w, coord_type h )
{
	uint16_t tw = static_cast<uint16_t>( w );
	uint16_t th = static_cast<uint16_t>( h );
	if ( _last_w != tw || _last_h != th )
	{
		_last_w = tw;
		_last_h = th;

		if ( _egl_win )
		{
			wl_egl_window_resize( _egl_win, _last_w, _last_h, 0, 0 );
			acquire();
			glViewport( 0, 0, tw, th );
			if ( resized )
				resized( w, h );
			release();
		}
	}
}

////////////////////////////////////////

void window::expose_event( coord_type x, coord_type y, coord_type w, coord_type h )
{
	_invalid = false;
	acquire();
	if ( exposed )
		exposed();
	eglSwapBuffers( _disp, _egl_surface );
	//glXSwapBuffers( _display.get(), _win );
	//glFlush();
	//XFlush( _display.get() );
	release();
}

////////////////////////////////////////

void
window::make_current( const std::shared_ptr<::platform::cursor> & )
{
	throw_not_yet();
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform

