// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "waitable.h"

////////////////////////////////////////

namespace platform
{
////////////////////////////////////////

waitable::waitable( system *s ) : event_source( s ) {}

////////////////////////////////////////

waitable::~waitable( void ) {}

////////////////////////////////////////

} // namespace platform
