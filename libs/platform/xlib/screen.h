//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <platform/screen.h>
#include <X11/Xlib.h>
#include <memory>
#include <color/color.h>

namespace platform
{

class renderer;

namespace xlib
{

////////////////////////////////////////

/// @brief X11 implementation of platform::screen
class screen : public ::platform::screen
{
public:
	/// @brief Constructor.
	screen( const std::shared_ptr<Display> &disp, int scr );
	~screen( void );

	bool is_default( void ) const override;

	bool is_managed( void ) const override;

	bool is_remote( void ) const override;

	rect bounds( bool avail ) const override;

	dots_per_unit dpi( void ) const override;
	dots_per_unit dpmm( void ) const override { return _d_p_mm; }

	double refresh_rate( void ) const override;

	const color::standard_definition &display_standard( void ) const override;
	void override_display_standard( const color::standard_definition & ) override;

	static int resolution_event_id();
	void update_resolution( void );

private:
	std::shared_ptr<Display> _display;
	int _screen = 0;

	color::standard_definition _standard;
	dots_per_unit _d_p_mm;
	double _d_rr = 0.0;
};

////////////////////////////////////////

} }

