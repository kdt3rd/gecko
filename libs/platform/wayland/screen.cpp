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

screen::screen( void )
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

base::rect screen::bounds( bool active ) const
{
	return base::rect( 0.0, 0.0, 1024.0, 720.0 );
}

////////////////////////////////////////

base::size screen::dpi( void ) const
{
	return { 95.0, 95.0 };
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform

