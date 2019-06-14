// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "screen.h"

#include <base/contract.h>
#include <stdexcept>
#include <string>

namespace platform
{
namespace dummy
{
////////////////////////////////////////

screen::screen( void ) {}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

core::size screen::bounds( void ) { return { 0, 0 }; }

////////////////////////////////////////

} // namespace dummy
} // namespace platform
