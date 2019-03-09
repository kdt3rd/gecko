// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "image.h"
#include <media/container.h>
#include <media/frame.h>
#include <vector>
#include <string>

namespace engine { class registry; }

////////////////////////////////////////

namespace image
{

image_buf extract_frame(
	const media::frame &f,
	const std::string &layer = std::string(),
	const std::string &view = std::string(),
	const std::vector<std::string> &planes = std::vector<std::string>() );

/// Simple image to a frame with a single default / unnamed (well, empty string) layer and one view
std::shared_ptr<media::frame> to_frame( const image_buf &i, const std::vector<std::string> &chans, const std::string &type, const media::metadata &meta = media::metadata() );

void debug_save_image( const image_buf &i, const std::string &fn, int64_t sampNum, const std::vector<std::string> &chans, const std::string &type, const media::parameter_set &params = media::parameter_set(), const media::metadata &meta = media::metadata() );

std::shared_ptr<media::frame> load_frame( const std::shared_ptr<media::container> &c, size_t videoTrackIdx, int64_t sampNum );
plane extract_view_plane( const std::shared_ptr<media::frame> &frame, const std::string &layer, const std::string &view, const std::string &p );
image_buf extract_view( const std::shared_ptr<media::frame> &frame, const std::string &layer, const std::string &view );

std::shared_ptr<media::frame> start_frame(
	const std::shared_ptr<media::container> &c,
	size_t videoTrackIdx,
	int64_t sampNum );

std::shared_ptr<media::frame> add_output_image(
	const std::shared_ptr<media::frame> &frame,
	const std::string &layer,
	const std::string &view,
	const image_buf &img,
	const std::vector<std::string> &planes );

engine::computed_value<bool> finish_frame( const std::shared_ptr<media::frame> &frame );

void add_media_io( engine::registry &r );

} // namespace image



