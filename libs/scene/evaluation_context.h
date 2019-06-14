// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

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
