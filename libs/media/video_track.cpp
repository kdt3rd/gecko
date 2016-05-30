//
// Copyright (c) 2016 Kimball Thurston
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

#include "video_track.h"


////////////////////////////////////////


namespace media
{

////////////////////////////////////////

video_track::~video_track( void )
{
}


////////////////////////////////////////


image_frame *
video_track::read( int64_t offset, const sample_rate &r )
{
	int64_t f = offset;
	if ( rate().valid() )
		f = rate().resample( f, r );

	return doRead( f );
}


////////////////////////////////////////


void
video_track::write( int64_t offset, const sample_rate &, const sample_data &sd )
{
	const image_frame *frm = dynamic_cast<const image_frame *>( &sd );
	if ( frm )
		doWrite( offset, *frm );
}


////////////////////////////////////////


} // media



