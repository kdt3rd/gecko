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

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void move( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void acquire( void ) override;
	void release( void ) override;

	double scale_factor( void );

	void resize_event( double w, double h );

	void invalidate( const base::rect &r ) override;

	void set_ns( void *nswin, void *nsview );

	double width( void ) override
	{
		return _last_w;
	}

	double height( void ) override
	{
		return _last_h;
	}

private:
	struct objcwrapper;
	std::unique_ptr<objcwrapper> _impl;

	double _last_w = 0.0, _last_h = 0.0;
};

////////////////////////////////////////

} }

