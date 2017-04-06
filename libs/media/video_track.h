//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "image_frame.h"
#include "track.h"

namespace media
{

////////////////////////////////////////

class video_track : public track
{
public:
	virtual ~video_track( void );

	using track::track;

	std::shared_ptr<image_frame> operator[]( int64_t f )
	{
		return at( f );
	}

	std::shared_ptr<image_frame> at( int64_t f )
	{
		return std::shared_ptr<image_frame>( doRead( f ) );
	}

	inline void store( int64_t f, const std::shared_ptr<image_frame> &frm )
	{
		if ( frm )
			doWrite( f, *frm );
	}

protected:
	friend class sample;

	virtual image_frame *read( int64_t offset, const sample_rate &r );
	virtual image_frame *doRead( int64_t offset ) = 0;

	virtual void write( int64_t offset, const sample_rate &r, const sample_data &sd );
	virtual void doWrite( int64_t offset, const image_frame &sd ) = 0;

private:
};

////////////////////////////////////////

}

