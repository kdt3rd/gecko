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

class context;

////////////////////////////////////////

/// @brief Cocoa implementation of window.
class window : public ::platform::window
{
public:
	window( const std::shared_ptr<::platform::screen> &s, const rect &p );
	~window( void ) override;

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

	double scale_factor( void );

	void set_ns( void *nsview );

protected:
	void make_current( const std::shared_ptr<cursor> & ) override;

	rect query_geometry( void ) override;
	bool update_geometry( rect &r ) override;

	void submit_delayed_expose( const rect &r ) override;

private:
	struct objcwrapper;
	std::unique_ptr<objcwrapper> _impl;
	std::shared_ptr<context> _context;
};

////////////////////////////////////////

} }

