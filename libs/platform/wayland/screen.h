//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/screen.h>
#include <memory>
#include <color/color.h>

namespace platform
{

class renderer;

namespace wayland
{

////////////////////////////////////////

/// @brief wayland implementation of platform::screen
class screen : public ::platform::screen
{
public:
	/// @brief Constructor.
	screen( const std::shared_ptr<::platform::renderer> &r );
	~screen( void );

	bool is_default( void ) const override;

	bool is_managed( void ) const override;

	rect bounds( bool active ) const override;

	base::dsize dpi( void ) const override;

	double refresh_rate( void ) const override;

	const std::shared_ptr<::platform::renderer> &render( void ) const override;

	const color::standard_definition &display_standard( void ) const override;
	void override_display_standard( const color::standard_definition & ) override;

private:
	std::shared_ptr<::platform::renderer> _render;
	color::standard_definition _standard;
};

////////////////////////////////////////

} }

