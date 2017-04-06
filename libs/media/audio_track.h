//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



