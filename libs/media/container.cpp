//
// Copyright (c) 2016 Kimball Thurston & Ian Godin
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

