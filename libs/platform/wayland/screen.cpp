//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <base/contract.h>

namespace platform
{
namespace wayland
{

////////////////////////////////////////

screen::screen( const std::shared_ptr<::platform::renderer> &r )
	: _render( r ), _standard( color::make_standard( color::standard::SRGB ) )
{
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

bool screen::is_default( void ) const
{
	return true;
}

////////////////////////////////////////

double screen::refresh_rate( void ) const
{
	return 30.0;
}

////////////////////////////////////////

rect screen::bounds( bool active ) const
{
	return rect( coord_type( 0 ), coord_type( 0 ), coord_type( 1024 ), coord_type( 720 ) );
}

////////////////////////////////////////

base::dsize screen::dpi( void ) const
{
	return { 95.0, 95.0 };
}

////////////////////////////////////////

bool
screen::is_managed( void ) const
{
	return true;
}

////////////////////////////////////////

const std::shared_ptr<::platform::renderer> &
screen::render( void ) const
{
	return _render;
}

////////////////////////////////////////

const color::standard_definition &
screen::display_standard( void ) const
{
	return _standard;
}

////////////////////////////////////////

void
screen::override_display_standard( const color::standard_definition &s )
{
	_standard = s;
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform

