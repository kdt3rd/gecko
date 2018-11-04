//
// Copyright (c) 2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include <base/uri.h>
#include <functional>
#include "track.h"
#include "video_track.h"
#include "audio_track.h"
#include "data_track.h"

#include "metadata.h"
#include "parameter.h"

namespace media
{

////////////////////////////////////////

/// @brief container is a media container.
///
/// A sample of this would be a quicktime file, or an MXF, or AVC,
/// etc. for single-frame image files this is sometimes clumsy,
/// however provides unified access to both types of media.
///
/// There is an attempt to have this all be generic, however to make
/// access a bit easier, tracks have been described as 3 types of
/// tracks:
///  - video track @sa video_track
///  - audio track @sa audio_track
///  - data track @sa data_track
///
/// Each level of item (container on down) will have metadata associated with it
/// 
///
class container
{
public:
//	container( void );
//	~container( void );

	size_t size( void )
	{
		return _tracks.size();
	}

	/// There is some amount of container info
	/// such as creator and such
	inline const metadata &info( void ) const;
	inline void add_info( base::cstring n, metadata_value v );

	std::shared_ptr<track> operator[]( size_t i )
	{
		return _tracks.at( i );
	}

	std::shared_ptr<track> at( size_t i )
	{
		return _tracks.at( i );
	}

	void add_track( const std::shared_ptr<track> &t );

	inline const std::vector<std::shared_ptr<video_track>> &video_tracks( void ) const;
	inline const std::vector<std::shared_ptr<audio_track>> &audio_tracks( void ) const;
	inline const std::vector<std::shared_ptr<data_track>> &data_tracks( void ) const;

	inline const parameter_set &parameters( void ) const { return _parameters; }
	void set_parameters( const parameter_set &p );

private:
	metadata _info;
	parameter_set _parameters;

	std::vector<std::shared_ptr<track>> _tracks;
	std::vector<std::shared_ptr<video_track>> _video_tracks;
	std::vector<std::shared_ptr<audio_track>> _audio_tracks;
	std::vector<std::shared_ptr<data_track>> _data_tracks;
};

////////////////////////////////////////

inline const metadata &container::info( void ) const
{ return _info; }
inline void container::add_info( base::cstring n, metadata_value v )
{ _info[n] = std::move( v ); }


////////////////////////////////////////

inline const std::vector<std::shared_ptr<video_track>> &container::video_tracks( void ) const
{
	return _video_tracks;
}

inline const std::vector<std::shared_ptr<audio_track>> &container::audio_tracks( void ) const
{
	return _audio_tracks;
}

inline const std::vector<std::shared_ptr<data_track>> &container::data_tracks( void ) const
{
	return _data_tracks;
}



////////////////////////////////////////

}

