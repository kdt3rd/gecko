//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/window.h>
#include <wayland-client.h>
#include <EGL/egl.h>
#include <wayland-egl.h>

namespace platform
{

class context;
class screen;

namespace wayland
{

class context;

////////////////////////////////////////

/// @brief Wayland implementation of platform::window.
class window : public ::platform::window
{
public:
	window( EGLDisplay disp, struct wl_compositor *comp, struct wl_shell *shell,
			const std::shared_ptr<::platform::screen> &scr );
	~window( void );

	::platform::context &hw_context( void ) override;

	void raise( void ) override;
	void lower( void ) override;
	void set_popup( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

	void fullscreen( bool fs ) override;

//	rect geometry( void ) override;
//	void set_position( coord_type x, coord_type y ) override;
	void move( coord_type x, coord_type y ) override;
	void resize( coord_type w, coord_type h ) override;
	void set_minimum_size( coord_type w, coord_type h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void invalidate( const rect &r ) override;

	coord_type width( void ) override { return _last_w; }
	coord_type height( void )  override { return _last_h; }

	void expose_event( coord_type x, coord_type y, coord_type w, coord_type h );
	void move_event( coord_type x, coord_type y );
	void resize_event( coord_type w, coord_type h );

protected:
	void make_current( const std::shared_ptr<::platform::cursor> & );

private:
	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;

	bool _invalid = false;
	bool _popup = false;

	std::shared_ptr<context> _ctxt;

	EGLDisplay _disp = nullptr;
	struct wl_surface *_surface = nullptr;
	struct wl_shell_surface *_shell_surf = nullptr;
	struct wl_egl_window *_win = nullptr;
	
};

////////////////////////////////////////

} }

