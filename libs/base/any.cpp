// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "any.h"

namespace base
{
////////////////////////////////////////

bad_any_cast::~bad_any_cast( void ) {}

////////////////////////////////////////

const char *bad_any_cast::what() const noexcept { return "bad any cast"; }

} // namespace base
