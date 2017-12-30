//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>

#include <platform/window.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of window.
class window : public ::platform::window
{
public:
	window( void );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;

	void show( void ) override;
	void hide( void ) override;
	void set_popup( void ) override;
	bool is_visible( void ) override;

	void fullscreen( bool fs ) override;

	void move( coord_type x, coord_type y ) override;
	void resize( coord_type w, coord_type h ) override;
	void set_minimum_size( coord_type w, coord_type h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void acquire( void ) override;
	void release( void ) override;

	double scale_factor( void );

	void invalidate( const rect &r ) override;

	void set_ns( void *nswin, void *nsview );

	coord_type width( void ) override
	{
		return _last_w;
	}

	coord_type height( void ) override
	{
		return _last_h;
	}

//protected:
	void make_current( const std::shared_ptr<cursor> & ) override;

	void expose_event( void ) override;
	void move_event( coord_type x, coord_type y ) override;
	void resize_event( coord_type w, coord_type h ) override;

private:
	struct objcwrapper;
	std::unique_ptr<objcwrapper> _impl;

	coord_type _last_w = 0, _last_h = 0;
};

////////////////////////////////////////

} }

