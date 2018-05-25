//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/window.h>
#include <windows.h>

namespace platform
{
namespace mswin
{

class context;

////////////////////////////////////////

class window : public ::platform::window
{
	using base = ::platform::window;

public:
	window( const std::shared_ptr<screen> &screen, const rect &p = rect( 0, 0, 512, 512 ) );
	~window( void );

	::platform::context &hw_context( void ) override;

	void raise( void ) override;
	void lower( void ) override;

	void set_popup( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

	void fullscreen( bool fs ) override;

	void set_minimum_size( coord_type w, coord_type h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	HWND id( void ) const { return _hwnd; }

protected:
	void make_current( const std::shared_ptr<cursor> &c ) override;

	rect query_geometry( void ) override;
	bool update_geometry( rect &r ) override;

	void submit_delayed_expose( const rect &r ) override;

private:
	HWND _hwnd;
	std::unique_ptr<context> _context;
};

////////////////////////////////////////

} }

