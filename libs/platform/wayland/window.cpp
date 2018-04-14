//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"

#include "context.h"

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

window::window( EGLDisplay disp, struct wl_compositor *comp, struct wl_shell *shell, const std::shared_ptr<::platform::screen> &scr )
	: ::platform::window( scr ), _disp( disp )
{
	_last_w = 512; _last_h = 512;

	TODO("add more error checks")
	_surface = wl_compositor_create_surface( comp );
	_shell_surf = wl_shell_get_shell_surface( shell, _surface );

	wl_shell_surface_add_listener( _shell_surf, &shell_listener, this );
	wl_shell_surface_set_toplevel( _shell_surf );
	_win = wl_egl_window_create( _surface, _last_w, _last_h );

	_ctxt = std::make_shared<context>( disp );
	_ctxt->create( (EGLNativeWindowType)_win );

	std::cout << "OpenGL:\n\tvendor " << glGetString( GL_VENDOR )
			  << "\n\trenderer " << glGetString( GL_RENDERER )
			  << "\n\tversion " << glGetString( GL_VERSION )
			  << "\n\tshader language " << glGetString( GL_SHADING_LANGUAGE_VERSION )
			  << "\n\n" << std::endl;

	if ( !gl3wIsSupported( 3, 3 ) )
		throw std::runtime_error( "opengl 3.3 not supported" );

	glClearColor( 0.15, 0.15, 0.15, 0.15 );
	glClear( GL_COLOR_BUFFER_BIT );

	_ctxt->swap_buffers();
}

////////////////////////////////////////

window::~window( void )
{
	_ctxt.reset();

	wl_egl_window_destroy( _win );
	wl_shell_surface_destroy( _shell_surf );
	wl_surface_destroy( _surface );
}

////////////////////////////////////////

::platform::context &window::hw_context( void )
{
	return *(_ctxt);
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

		if ( _win )
		{
			wl_egl_window_resize( _win, _last_w, _last_h, 0, 0 );

			auto guard = _ctxt->begin_render();
			_ctxt->set_viewport( 0, 0, tw, th );
			if ( resized )
				resized( w, h );
		}
	}
}

////////////////////////////////////////

void window::expose_event( coord_type x, coord_type y, coord_type w, coord_type h )
{
	_invalid = false;
	auto guard = _ctxt->begin_render();
	if ( exposed )
		exposed();
	_ctxt->swap_buffers();
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

