// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "video_track.h"

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

video_track::~video_track( void ) {}

////////////////////////////////////////

frame *video_track::read( int64_t offset, const sample_rate &r )
{
    int64_t f = offset;
    if ( rate().valid() )
        f = rate().resample( f, r );

    return doRead( f );
}

////////////////////////////////////////

//void
//video_track::write( int64_t offset, const sample_rate &, const sample_data &sd )
//{
//	const frame *frm = dynamic_cast<const frame *>( &sd );
//	if ( frm )
//		doWrite( offset, *frm );
//}

////////////////////////////////////////

} // namespace media
