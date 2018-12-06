//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

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



