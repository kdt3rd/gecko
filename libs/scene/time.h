// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <media/sample_rate.h>

////////////////////////////////////////

namespace scene
{
using sample_rate = media::sample_rate;

class time
{
public:
    int64_t            offset( void ) const;
    double             sub_frame( void ) const;
    const sample_rate &rate( void ) const;
};

} // namespace scene
