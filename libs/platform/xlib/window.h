
#pragma once

#include <platform/window.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::window.
class window : public platform::window
{
public:
	/// @brief Constrcutor
	window( const std::shared_ptr<Display> &dpy );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;
	void set_popup( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void invalidate( const base::rect &r ) override;

	/// @brief Canvas to draw on the window.
	gl::context context( void ) override;
	std::shared_ptr<draw::canvas> canvas( void ) override;

	/// @brief Xlib window identifier.
	Window id( void ) const;
	void set_input_context( XIC xic ) { _xic = xic; }
	XIC input_context( void ) const { return _xic; }

	double width( void ) override { return _last_w; }
	double height( void )  override { return _last_h; }

	void expose_event( void );
	void move_event( double x, double y );
	void resize_event( double w, double h );

private:
	std::shared_ptr<Display> _display;
	Window _win = 0;
	XIC _xic = 0;

	std::shared_ptr<draw::canvas> _canvas;
	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;

	bool _invalid = false;

	GLXContext _glc;
};

////////////////////////////////////////

}

