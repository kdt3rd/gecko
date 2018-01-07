//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/window.h>
#include <windows.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

class window : public ::platform::window
{
public:
	window( void );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;

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
	void set_popup( void ) override;

	void invalidate( const rect &r );

	void acquire( void ) override;
	void release( void ) override;

	bool check_last_position( int16_t x, int16_t y ) { if ( _last_x != x || _last_y != y ) { _last_x = x; _last_y = y; return true; } return false; }
	bool check_last_size( uint16_t w, uint16_t h ) { if ( _last_w != w || _last_h != h ) { _last_w = w; _last_h = h; return true; } return false; }
	void update_position( void );

	HWND id( void ) const { return _hwnd; }

	coord_type width( void ) override { return _last_w; }
	coord_type height( void )  override { return _last_h; }
	// TODO: restore this once we are dispatching events
//protected:
	void make_current( const std::shared_ptr<cursor> &c ) override;
	void expose_event( coord_type x, coord_type y, coord_type w, coord_type h ) override;
	void move_event( coord_type x, coord_type y ) override;
	void resize_event( coord_type w, coord_type h ) override;

private:
	HWND _hwnd;
	HGLRC _hrc;
	HDC _hdc;

	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;
};

////////////////////////////////////////

} }

