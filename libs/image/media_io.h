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

#pragma once

#include "image.h"
#include <media/container.h>
#include <media/image_frame.h>
#include <vector>
#include <string>

namespace engine { class registry; }

////////////////////////////////////////

namespace image
{

image_buf extract_frame( const media::image_frame &f );
image_buf extract_frame( const media::image_frame &f, const std::vector<std::string> &chans );

std::shared_ptr<media::image_frame> to_frame( const image_buf &i, const std::vector<std::string> &chans, const std::string &type );

void debug_save_image( const image_buf &i, const std::string &fn, int64_t sampNum, const std::vector<std::string> &chans, const std::string &type, const media::metadata &options = media::metadata() );

image_buf load_frame( const std::shared_ptr<media::container> &c, size_t videoTrackIdx, int64_t sampNum );
engine::computed_value<bool> save_frame(
	const image_buf &i,
	const std::vector<std::string> &chans,
	const std::string &type,
	const std::shared_ptr<media::container> &c,
	size_t videoTrackIdx,
	int64_t sampNum );

void add_media_io( engine::registry &r );

} // namespace image



