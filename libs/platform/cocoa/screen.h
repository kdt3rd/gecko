//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/screen.h>
#include <color/standards.h>

namespace platform { namespace cocoa
{

class system;

////////////////////////////////////////

/// @brief Cocoa implementation of screen.
class screen : public platform::screen
{
public:
	screen( void ) = delete;
	screen( void *scr );
	~screen( void ) override;

	bool is_default( void ) const override;
	bool is_managed( void ) const override;

	bool is_remote( void ) const override;

	rect bounds( bool avail ) const override;
	dots_per_unit dpi( void ) const override;
	dots_per_unit dpmm( void ) const override;

	double refresh_rate( void ) const override;

	const color::standard_definition &display_standard( void ) const override;
	void override_display_standard( const color::standard_definition &s ) override;

private:
	friend class platform::cocoa::system;

	void *_nsscreen;
	color::standard_definition _standard;
	bool _override_standard = false;
};

////////////////////////////////////////

} }

