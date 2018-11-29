//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "track.h"
#include "frame.h"

namespace base
{
class allocator;
}

namespace media
{

////////////////////////////////////////

/// @brief video specialized variant of media track.
///
/// A video track is mean to encapsulate picture data. This may be
/// encoded as per-frame (perhaps intra-frame encoded / compressed)
/// data, or inter-frame encoded (i.e. typical mpeg encoded video).
///
/// A video track has a time range (inherited from track), but then is
/// expected to provide discrete frames of data upon request.
/// 
///
class video_track : public track
{
public:
	virtual ~video_track( void );

	// inherit constructor
	using track::track;

	std::shared_ptr<frame> operator[]( int64_t f )
	{
		return at( f );
	}

	std::shared_ptr<frame> at( int64_t f )
	{
		return std::shared_ptr<frame>( doRead( f ) );
	}

	inline void store( int64_t f, const std::shared_ptr<frame> &frm, base::allocator &a )
	{
		if ( frm )
			doWrite( f, *frm, a );
	}

	// to add
	bool interframe_encoded( void ) const;
//	video_info info( void ) const
	
protected:
	video_track( void ) = default;
	// non-copyable
	video_track( const video_track & ) = delete;
	video_track &operator=( const video_track & ) = delete;
	video_track( video_track && ) = delete;
	video_track &operator=( video_track && ) = delete;

	friend class sample;

	virtual frame *read( int64_t offset, const sample_rate &r );
	virtual frame *doRead( int64_t offset ) = 0;

//	virtual void write( int64_t offset, const sample_rate &r, const sample_data &sd );
	virtual void doWrite( int64_t offset, const frame &sd, base::allocator &a ) = 0;

private:
};

////////////////////////////////////////

}

