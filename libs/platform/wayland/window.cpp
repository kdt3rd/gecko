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
	win->shell_resize_event( platform::coord_type( width ), platform::coord_type( height ) );
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

window::window( EGLDisplay disp, struct wl_compositor *comp, struct wl_shell *shell, const std::shared_ptr<::platform::screen> &scr, const rect &p )
	: ::platform::window( scr, p ), _disp( disp )
{
	TODO("add more error checks")
	_surface = wl_compositor_create_surface( comp );
	_shell_surf = wl_shell_get_shell_surface( shell, _surface );

	wl_shell_surface_add_listener( _shell_surf, &shell_listener, this );
	wl_shell_surface_set_toplevel( _shell_surf );
	_win = wl_egl_window_create( _surface, width(), height() );

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

void window::apply_minimum_size( coord_type w, coord_type h )
{
	_min_w = w;
	_min_h = h;
	rect r = query_geometry();
	bool changed = false;
	if ( r.width() < _min_w )
	{
		r.set_width( _min_w );
		changed = true;
	}
	if ( r.height() < _min_h )
	{
		r.set_height( _min_h );
		changed = true;
	}
	if ( changed )
		update_geometry( r );
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
}

////////////////////////////////////////

void window::shell_resize_event( coord_type w, coord_type h )
{
	if ( _win )
		wl_egl_window_resize( _win, w, h, 0, 0 );
}

////////////////////////////////////////

void
window::make_current( const std::shared_ptr<::platform::cursor> & )
{
	throw_not_yet();
}

////////////////////////////////////////

void window::submit_delayed_expose( const rect &r )
{
	wl_surface_damage( _surface, r.x(), r.y(), r.width(), r.height() );
}

////////////////////////////////////////

rect window::query_geometry( void )
{
	rect ret;
	int w = 0, h = 0;
	wl_egl_window_get_attached_size( _win, &w, &h );
	ret.set_size( w, h );
	_delay_position = ret;
	return ret;
}

////////////////////////////////////////

bool window::update_geometry( rect &r )
{
	if ( r.width() < _min_w )
		r.set_width( _min_w );
	if ( r.height() < _min_h )
		r.set_height( _min_h );

	wl_egl_window_resize( _win, r.width(), r.height(), r.x(), r.y() );
	return true;
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform

