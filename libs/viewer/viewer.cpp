//
// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "viewer.h"
#include <viewer/shaders.h>
#include <media/reader.h>
#include <algorithm>

////////////////////////////////////////

namespace gui
{

////////////////////////////////////////

viewer_w::viewer_w( void )
	: _rect( { 0.26, 0.26, 0.26 } )
{
	_rect.set_color( gl::color( 0.21, 0.21, 0.21, 1.0 ) );
}

////////////////////////////////////////

size_t viewer_w::add_video_track( const std::shared_ptr<media::video_track> &t )
{
	size_t id = ++_cur_ID;
	ImageSet &nset = _images[id];
	nset._source = t;
	nset._active = true;
	_draw_order.push_back( id );
	return id;
}

////////////////////////////////////////

void viewer_w::remove_video_track( const std::shared_ptr<media::video_track> &t )
{
	auto i = _images.begin();
	while ( i != _images.end() )
	{
		if ( i->second._source == t )
		{
			// yeah, we'll re-search for the item we already have, but the draw order
			// logic is easier to maintain in one place
			remove_item( i->first );
			i = _images.begin();
			continue;
		}

		++i;
	}
}

////////////////////////////////////////

void viewer_w::remove_item( size_t id )
{
	auto i = _images.find( id );
	if ( i != _images.end() )
		_images.erase( i );

	for ( auto oi = _draw_order.begin(); oi != _draw_order.end(); ++oi )
	{
		if ( (*oi) == id )
		{
			_draw_order.erase( oi );
			break;
		}
	}

	invalidate();
}

////////////////////////////////////////

void viewer_w::set_active( size_t id, bool act )
{
	auto i = _images.find( id );
	if ( i != _images.end() )
	{
		bool doupd = act && ! i->second._active;
		i->second._active = act;
		if ( doupd )
			update_images( false );
	}
}

////////////////////////////////////////

void viewer_w::stack_items( size_t a, size_t b )
{
	auto aci = _images.find( a );
	auto bci = _images.find( b );
	if ( aci != _images.end() && bci != _images.end() )
	{
		ImageSet &as = aci->second;
		ImageSet &bs = aci->second;

		if ( as._above != kInvalidID && as._above != b )
		{
			auto oi = _images.find( as._above );
			if ( oi != _images.end() )
				oi->second._below = kInvalidID;
		}
		if ( bs._below != kInvalidID && bs._below != a )
		{
			auto oi = _images.find( bs._below );
			if ( oi != _images.end() )
				oi->second._above = kInvalidID;
		}
		as._above = b;
		bs._below = a;

		auto bi = _draw_order.end();
		auto ai = _draw_order.end();
		for ( auto oi = _draw_order.begin(); oi != _draw_order.end(); ++oi )
		{
			if ( (*oi) == b )
			{
				if ( ai != _draw_order.end() )
				{
					++oi;
					_draw_order.erase( ai );
					_draw_order.insert( oi, a );
					break;
				}
				bi = oi;
			}

			if ( (*oi) == a )
			{
				// b is already being drawn first, leave it alone
				if ( bi != _draw_order.end() )
					break;
				ai = oi;
				// keep looking for b so we can put it a right after
			}
		}
	}
	// TODO: what to do if the user passes an invalid id?

	invalidate();
}

////////////////////////////////////////

void viewer_w::unstack_all( void )
{
	for ( auto &i: _images )
	{
		ImageSet &curs = i.second;
		curs._above = kInvalidID;
		curs._below = kInvalidID;
	}
	invalidate();
}

////////////////////////////////////////

void viewer_w::reset_positions( void )
{
	for ( auto &i: _images )
	{
		const auto &img = i.second._image;
		if ( img )
			img->reset_position( width().count(), height().count() );
	}
	invalidate();
}

////////////////////////////////////////

void viewer_w::set_filtering( draw::zoom_filter f )
{
	_filter = f;
	for ( auto &i: _images )
	{
		const auto &img = i.second._image;
		if ( img )
			img->set_filtering( f );
	}
	invalidate();
}

////////////////////////////////////////

void viewer_w::update_frame( const media::sample &s, bool force_reload )
{
	_current_sample = s;

	update_images( force_reload );

	invalidate();
}

////////////////////////////////////////

void viewer_w::build( context &ctxt )
{
}

////////////////////////////////////////

void viewer_w::paint( context &ctxt )
{
	platform::context &hwc = ctxt.hw_context();

	_rect.set_position( x(), y() );
	_rect.set_size( width(), height() );
	_rect.draw( hwc );

	// TODO: need to manage wipe...
	int texoff = 0;
	for ( auto &i: _draw_order )
	{
		auto ci = _images.find( i );
		if ( ci != _images.end() )
		{
			ImageSet &curs = ci->second;
			const auto &img = curs._image;
			if ( curs._active && img )
			{
				img->set_texture_offset( texoff );
				img->draw( hwc );
				texoff += img->num_textures();
			}
		}
	}
}

////////////////////////////////////////

bool viewer_w::mouse_press( const event &e )
{
	if ( e.raw_mouse().button == 1 )
	{
		_panning = true;
		_last = e.from_native( e.raw_mouse().x, e.raw_mouse().y );
		context::current().grab_source( e, shared_from_this() );

		return true;
	}
//	else if ( button == 2 )
//	{
//		_panningB = true;
//		_last = p;
//		return true;
//	}
	return widget::mouse_press( e );
}

////////////////////////////////////////

bool viewer_w::mouse_move( const event &e )
{
	// TODO: fix this
	if ( _panning )
	{
		point p = e.from_native( e.raw_mouse().x, e.raw_mouse().y );
		point delta = ( p - _last );
		for ( auto &i: _images )
		{
			const auto &img = i.second._image;
			if ( img )
				img->set_pan( delta[0].count(), delta[1].count() );
		}

		_last = p;
		invalidate();
		return true;
	}
	else
		_last = e.from_native( e.raw_mouse().x, e.raw_mouse().y );

	return widget::mouse_move( e );
}

////////////////////////////////////////

bool viewer_w::mouse_release( const event &e )
{
	if ( _panning )
	{
		context::current().release_source( e );
		_panning = false;
		return true;
	}

	return widget::mouse_release( e );
}

////////////////////////////////////////

bool viewer_w::mouse_wheel( const event &e )
{
	int amount = e.raw_hid().position;
	// TODO: fix this
	float zoomF = amount > 0 ? 2.F : 0.5F;
	// TODO: need the real event to get the mouse position to zoom around that
	float pivx = _last[0].count(), pivy = _last[1].count();
	for ( auto &i: _images )
	{
		const auto &img = i.second._image;
		if ( img )
			img->add_zoom( pivx, pivy, zoomF );
	}

	invalidate();
	return true;
}

////////////////////////////////////////

bool viewer_w::key_release( const event &e )
{
	using namespace platform;
	scancode c = e.raw_key().keys[0];

	switch ( c )
	{
		case scancode::KEY_ESCAPE: reset_positions(); return true;
		case scancode::KEY_L:
			if ( _filter == draw::zoom_filter::nearest )
				set_filtering( draw::zoom_filter::linear );
			else
				set_filtering( draw::zoom_filter::nearest );
			return true;

		case scancode::KEY_F:
			std::rotate( _draw_order.begin(), std::next( _draw_order.begin() ), _draw_order.end() );
			invalidate();
			return true;
		default:
			break;
	}
	return widget::key_release( e );
}

////////////////////////////////////////

void viewer_w::update_images( bool force_reload )
{
	gl::api &ogl = context::current().hw_context().api();

	for ( auto &i: _images )
	{
		ImageSet &curs = i.second;
		if ( curs._active && curs._source &&
			 ( force_reload || curs._last_loaded != _current_sample ) )
		{
			try
			{
				curs._cur = _current_sample( curs._source );
				if ( ! curs._image )
					curs._image = std::make_shared<draw::image>();
				curs._image->convert( ogl, *(curs._cur) );
				curs._image->set_filtering( _filter );
				curs._last_loaded = _current_sample;
			}
			catch ( std::exception &e )
			{
				std::cerr << "Unable to read sample " << _current_sample << " for image ID " << i.first << ": " << e.what() << std::endl;
				curs._image->clear();
				// TBD: set an error frame...
			}
		}
	}

}

////////////////////////////////////////

}
