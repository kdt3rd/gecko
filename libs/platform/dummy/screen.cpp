//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <base/contract.h>
#include <string>
#include <stdexcept>

namespace platform { namespace dummy
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

core::size screen::bounds( void )
{
	return { 0, 0 };
}

////////////////////////////////////////

} }

