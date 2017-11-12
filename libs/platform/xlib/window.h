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

namespace platform { namespace xlib
{

class system;

////////////////////////////////////////

/// @brief Xlib implementation of platform::window.
class window : public ::platform::window
{
public:
	/// @brief Constrcutor
	window( system &s, const std::shared_ptr<Display> &dpy );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;
	void set_popup( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

	void fullscreen( bool fs ) override;

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void move( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void invalidate( const base::rect &r ) override;

	/// @brief Acquire window to draw
	void acquire( void ) override;
	void release( void ) override;

	/// @brief Xlib window identifier.
	Window id( void ) const;
	void set_input_context( XIC xic ) { _xic = xic; }
	XIC input_context( void ) const { return _xic; }

	double width( void ) override { return _last_w; }
	double height( void )  override { return _last_h; }

protected:
	void make_current( const std::shared_ptr<::platform::cursor> & );

	void expose_event( void ) override;
	void move_event( double x, double y ) override;
	void resize_event( double w, double h ) override;

private:
	std::shared_ptr<Display> _display;
	Window _win = 0;
	XIC _xic = 0;

	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;

	bool _invalid = false;
	bool _popup = false;
	bool _fullscreen = false;

	GLXContext _glc;
	void (*_glc_makecurrent)( Display *, GLXDrawable, GLXContext ) = nullptr;
	void (*_glc_swapbuffers)( Display *, GLXDrawable ) = nullptr;
};

////////////////////////////////////////

} }

