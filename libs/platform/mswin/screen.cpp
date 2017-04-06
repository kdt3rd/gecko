//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <string>
#include <stdexcept>

namespace platform { namespace mswin
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

base::size screen::bounds( void ) const
{
	return { 0, 0 };
}

////////////////////////////////////////

} }

