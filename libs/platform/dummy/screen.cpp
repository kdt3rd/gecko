// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

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
