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

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void move( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;
	void set_popup( void ) override;

	void invalidate( const base::rect &r );

	void acquire( void ) override;
	void release( void ) override;

	bool check_last_position( int16_t x, int16_t y ) { if ( _last_x != x || _last_y != y ) { _last_x = x; _last_y = y; return true; } return false; }
	bool check_last_size( uint16_t w, uint16_t h ) { if ( _last_w != w || _last_h != h ) { _last_w = w; _last_h = h; return true; } return false; }
	void update_position( void );

	HWND id( void ) const { return _hwnd; }

	double width( void ) override { return _last_w; }
	double height( void )  override { return _last_h; }

	void expose_event( void );

private:
	HWND _hwnd;
	HGLRC _hrc;
	HDC _hdc;

	int16_t _last_x = 0, _last_y = 0;
	uint16_t _last_w = 0, _last_h = 0;
};

////////////////////////////////////////

} }

