// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "time.h"

////////////////////////////////////////

namespace scene
{
class evaluation_context
{
public:
    const time &current_time( void ) const;
    time        offset_time( double offset ) const;
};

} // namespace scene
