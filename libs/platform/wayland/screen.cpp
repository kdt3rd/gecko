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

base::size screen::bounds( void ) const
{
	return { 1024.0, 720.0 };
}

////////////////////////////////////////

base::size screen::dpi( void ) const
{
	return { 99.0, 99.0 };
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform

