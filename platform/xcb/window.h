
#pragma once

#include <platform/window.h>
#include <draw/cairo/canvas.h>
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

/// @brief XCB implementation of platform::window.
class window : public platform::window
{
public:
	/// @brief Constrcutor
	window( xcb_connection_t *c, xcb_screen_t *screen );
	virtual ~window( void );

	virtual void raise( void );
//	virtual void lower( void );

	virtual void show( void );
	virtual void hide( void );
	virtual bool is_visible( void );

//	virtual rect geometry( void );
//	virtual void set_position( double x, double y );
	virtual void resize( double w, double h );
	virtual void set_minimum_size( double w, double h );

	virtual void set_title( const std::string &t );
//	virtual void set_icon( const icon &i );

	virtual void invalidate( const draw::rect &r );

	/// @brief Canvas to draw on the window.
	virtual std::shared_ptr<draw::canvas> canvas( void );

	/// @brief XCB window identifier.
	xcb_window_t id( void ) const;

	/// @brief Check if the previous position has changed.
	bool check_last_position( int16_t x, int16_t y ) { if ( _last_x != x || _last_y != y ) { _last_x = x; _last_y = y; return true; } return false; }

	/// @brief Check if the previous size has changed.
	bool check_last_size( uint16_t w, uint16_t h ) { if ( _last_w != w || _last_h != h ) { _last_w = w; _last_h = h; return true; } return false; }

	/// @brief Called when the window has resized.
	void resize_canvas( double w, double h );

	virtual double width( void ) { return _last_w; }
	virtual double height( void ) { return _last_h; }

private:
	void update_canvas( double w, double h );

	xcb_connection_t *_connection = nullptr;
	xcb_screen_t *_screen = nullptr;
	xcb_visualtype_t *_visual = nullptr;
	xcb_window_t _win = 0;

	std::shared_ptr<cairo::canvas> _canvas;
	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;
};

////////////////////////////////////////

}

