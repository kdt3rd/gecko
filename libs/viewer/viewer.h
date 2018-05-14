//
// Copyright (c) 2014-2018 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gui/widget.h>
#include "image.h"
#include <draw/rectangle.h>
#include <media/sample.h>
#include <map>
#include <list>

namespace media
{
class video_track;
class sample;
}

// TODO: should we put it in it's own namespace???
namespace gui
{

////////////////////////////////////////

class viewer_w : public gui::widget
{
public:
	viewer_w( void );

	// add image sources to the viewer
	size_t add_video_track( const std::shared_ptr<media::video_track> &t );
//	size_t add_image( );

	// remove image sources from the viewer
	void remove_video_track( const std::shared_ptr<media::video_track> &t );
	void remove_item( size_t id );

	void set_active( size_t id, bool act );

	void stack_items( size_t a, size_t b );

	void unstack_items( size_t a, size_t b );
	void unstack_all( void );

	// reset all the pan / zoom positions
	void reset_positions( void );

	void set_filtering( draw::zoom_filter f );

	void update_frame( const media::sample &s, bool force_reload = false );

	void build( context &ogl ) override;
	void paint( context &ctxt ) override;

	bool mouse_press( const event &e ) override;
	bool mouse_move( const event &e ) override;
	bool mouse_release( const event &e ) override;
	bool mouse_wheel( const event &e ) override;
	bool key_release( const event &e ) override;

private:
	void update_images( bool force_reload );

	media::sample _current_sample;

	static const size_t kInvalidID = size_t(-1);

	struct ImageSet
	{
		std::shared_ptr<media::video_track> _source;
		media::sample _last_loaded;
		std::shared_ptr<media::image_frame> _cur;
		std::shared_ptr<draw::image> _image;
		bool _active = false;
		size_t _above = kInvalidID;
		size_t _below = kInvalidID;
	};

	draw::zoom_filter _filter = draw::zoom_filter::nearest;
	size_t _cur_ID = 0;
	std::map<size_t, ImageSet> _images;
	std::list<size_t> _draw_order;

	draw::rectangle _rect;

	bool _panning = false;
	point _last;
};

////////////////////////////////////////

using viewer = widget_ptr<viewer_w>;

////////////////////////////////////////

} // namespace gui

