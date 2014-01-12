
#pragma once

#include <platform/window.h>
#include <cairo/cairo-gl.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace cairo
{
	class canvas;
}

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::window.
class window : public platform::window
{
public:
	/// @brief Constrcutor
	window( Display *dpy );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void invalidate( const draw::rect &r ) override;

	/// @brief Canvas to draw on the window.
	gl::context context( void ) override;
	std::shared_ptr<draw::canvas> canvas( void ) override;

	/// @brief Xlib window identifier.
	Window id( void ) const;

	double width( void ) override { return _last_w; }
	double height( void )  override { return _last_h; }

	void expose_event( void );
	void move_event( double x, double y );
	void resize_event( double w, double h );

private:
	Display *_display = nullptr;
	Window _win = 0;

	std::shared_ptr<cairo::canvas> _canvas;
	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;

	GLXContext _glc;
	cairo_device_t *_device = nullptr;
};

////////////////////////////////////////

}

