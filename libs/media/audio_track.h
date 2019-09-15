// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "track.h"

////////////////////////////////////////

namespace media
{
class audio_track : public track
{
public:
    using track::track;
    virtual ~audio_track( void );

private:
};

} // namespace media
