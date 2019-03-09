// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

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
	window( window_type wt, EGLDisplay disp, struct wl_compositor *comp, struct wl_shell *shell,
			const std::shared_ptr<::platform::screen> &scr,
			const rect &p = rect( 0, 0, 320, 243 ) );
	~window( void );

	::platform::context &hw_context( void ) override;

	void raise( void ) override;
	void lower( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

	void fullscreen( bool fs ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void shell_resize_event( coord_type w, coord_type h );
protected:
	void apply_minimum_size( coord_type w, coord_type h ) override;

	void make_current( const std::shared_ptr<::platform::cursor> & );

	void submit_delayed_expose( const rect &r ) override;
	rect query_geometry( void ) override;
	bool update_geometry( rect &r ) override;

private:
	coord_type _min_w = 0;
	coord_type _min_h = 0;
	rect _delay_position;

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

