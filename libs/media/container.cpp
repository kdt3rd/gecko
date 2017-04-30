//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "container.h"

namespace media
{


////////////////////////////////////////


// virtual destructors we avoid having in headers to
// avoid warnings
sample_data::~sample_data( void ) {}
track::~track( void ) {}
audio_track::~audio_track( void ) {}
data_track::~data_track( void ) {}
image_frame::~image_frame( void ) {}

////////////////////////////////////////

void
image_frame::copy_meta( const metadata &m )
{
	for ( const auto &i: m )
		_metadata[i.first] = i.second;
}


////////////////////////////////////////


void
container::add_track( const std::shared_ptr<track> &t )
{
	_tracks.push_back( t );

	auto vtrk = std::dynamic_pointer_cast<media::video_track>( t );
	if ( vtrk )
		_video_tracks.push_back( vtrk );

	auto atrk = std::dynamic_pointer_cast<media::audio_track>( t );
	if ( atrk )
		_audio_tracks.push_back( atrk );

	auto dtrk = std::dynamic_pointer_cast<media::data_track>( t );
	if ( dtrk )
		_data_tracks.push_back( dtrk );
}

////////////////////////////////////////

}

