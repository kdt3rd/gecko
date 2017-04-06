//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



