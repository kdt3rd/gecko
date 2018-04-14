//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/window.h>
#include <X11/Xlib.h>
#include <gl/opengl.h>
#include <GL/glx.h>

namespace platform
{

class context;

namespace xlib
{

class system;
class screen;
class context;

////////////////////////////////////////

/// @brief Xlib implementation of platform::window.
class window : public ::platform::window
{
public:
	/// @brief Constrcutor
	window( system &s, const std::shared_ptr<Display> &dpy, const std::shared_ptr<::platform::screen> &scr );
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

	/// @brief Xlib window identifier.
	Window id( void ) const;
	void set_input_context( XIC xic ) { _xic = xic; }
	XIC input_context( void ) const { return _xic; }

	coord_type width( void ) override { return _last_w; }
	coord_type height( void )  override { return _last_h; }

protected:
	void make_current( const std::shared_ptr<::platform::cursor> & );

	void expose_event( coord_type x, coord_type y, coord_type w, coord_type h ) override;
	void move_event( coord_type x, coord_type y ) override;
	void resize_event( coord_type w, coord_type h ) override;

private:
	std::shared_ptr<Display> _display;
	Window _win = 0;
	XIC _xic = 0;

	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;

	rect _invalid_rgn;
	bool _invalid = false;

	bool _popup = false;
	bool _fullscreen = false;

	std::shared_ptr<context> _ctxt;

	GLXContext _glc;

};

////////////////////////////////////////

} }

