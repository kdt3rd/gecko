//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "media_io.h"
#include "allocator.h"
#include <base/contract.h>
#include <media/writer.h>
#include <media/frame.h>
#include <media/image.h>
#include <media/image_buffer.h>

////////////////////////////////////////

namespace
{

void update_image_buffer(
	media::image_buffer &buf,
	const media::plane_layout &pl,
	const media::area_rect &area )
{
	if ( area.x1() == buf.x1() && area.x2() == buf.x2() &&
		 area.y1() == buf.y1() && area.y2() == buf.y2() &&
		 buf.offset() == 0 )
	{
		int64_t bytes = static_cast<int64_t>( ( int64_t( pl._bits ) + 7 ) / 8 );
		int64_t ystride = ( area.width() * bytes ) >> int64_t( pl._xsubsample_shift );
		if ( buf.bits() == int64_t( pl._bits ) &&
			 buf.xstride_bytes() == ( int64_t( pl._bits ) / 8 ) &&
			 buf.ystride_bytes() == ystride )
		{
			// same buffer size, we can just return and re-use the buffer
			return;
		}
	}
	buf = media::image_buffer::full_plane( area.x1(), area.y1(),
										   area.x2(), area.y2(),
										   pl._bits,
										   pl._xsubsample_shift,
										   pl._ysubsample_shift,
										   pl._floating, pl._unsigned );
}

class frame_image_adapter : public ::media::image
{
public:
	using base = ::media::image;
	using media_buf = ::media::image_buffer;
	using img_buf = ::image::image_buf;
	using rect = ::media::area_rect;

	frame_image_adapter(
		const img_buf &img,
		const std::vector<std::string> &chans,
		const std::string &type,
		const media::metadata &meta
						)
		: base( rect::from_points( img.x1(), img.y1(), img.x2(), img.y2() ) )
	{
		for ( size_t c = 0, nC = chans.size(); c != nC; ++c )
		{
			media::plane_layout pl;
			if ( type == "f16" )
			{
				pl._bits = 16;
				pl._floating = true;
				pl._unsigned = false;
			}
			else if ( type == "u16" )
			{
				pl._bits = 16;
				pl._floating = false;
				pl._unsigned = true;
			}
			else
				throw_runtime( "Unknown/unhandled data type tag {0}", type );
			register_plane( chans[c], pl, 0.0 );
			_image.add_plane( img[c] );
		}

		for ( auto &mv: meta )
			set_meta( mv.first, mv.second );
	}

protected:
	// TODO: query this from the media container
	bool storage_interleaved( void ) const { return false; }
	void fill_plane( size_t pIdx, media_buf &buffer ) override
	{
		precondition( pIdx < _image.size(), "Invalid plane {0} requested ({1} available)", pIdx, _image.size() );

		const ::image::plane &p = _image[pIdx];
		if ( buffer.x1() != p.x1() || buffer.x2() != p.x2() )
			throw_not_yet();

		for ( int64_t y = buffer.y1(), ly = buffer.y2(); y <= ly; ++y )
		{
			const float *pL = p.line( static_cast<int>( y ) );
			buffer.set_scanline( y, pL, 1 );
		}
	}
	void fill_image( std::vector<media_buf> &planes ) override
	{
		precondition( planes.size() == size(), "expect all planes for output image" );
		for ( size_t i = 0; i != planes.size(); ++i )
			fill_plane( i, planes[i] );
	}

private:
	img_buf _image;
};

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

image_buf extract_frame(
	const media::frame &f,
	const std::string &layer,
	const std::string &view,
	const std::vector<std::string> &planes )
{
	image_buf r;
	std::shared_ptr<media::image> img = f.find_image( layer, view );
	if ( img )
	{
		const media::area_rect active = img->active_area();
		int64_t dx1 = active.x1(), dx2 = active.x2();
		int64_t dy1 = active.y1(), dy2 = active.y2();

		if ( img->interleaved() )
		{
			// all planes should be the same
			// TODO: how do we handle 4:2:2 interleaved?
			throw_not_yet();
		}
		else if ( planes.empty() )
		{
			media::image_buffer tmp;
			for ( size_t p = 0, nP = img->size(); p != nP; ++p )
			{
				update_image_buffer( tmp, img->layout( p ), active );
				img->extract_plane( p, tmp );
				plane pl( dx1, dy1, dx2, dy2 );
				for ( int64_t y = dy1; y <= dy2; ++y )
					tmp.get_scanline( y, pl.line( static_cast<int>( y ) ), 1 );
				r.add_plane( std::move( pl ) );
			}
		}
		else
		{
			std::vector<size_t> pMapping;
			pMapping.reserve( planes.size() );

			media::image_buffer tmp;
			for ( auto &plane: planes )
			{
				bool found = false;
				for ( size_t p = 0, nP = img->size(); p != nP; ++p )
				{
					if ( img->plane_name( p ) == plane )
					{
						pMapping.push_back( p );
						found = true;
						break;
					}
				}
				if ( ! found )
					throw_runtime( "Request for channel '{0}' in layer '{0}', view '{1}' does not exist in frame {2}",
								   plane, layer, view, f.number() );
			}

			for ( size_t idx: pMapping )
			{
				update_image_buffer( tmp, img->layout( idx ), active );
				img->extract_plane( idx, tmp );
				plane pl( dx1, dy1, dx2, dy2 );
				for ( int64_t y = dy1; y <= dy2; ++y )
					tmp.get_scanline( y, pl.line( static_cast<int>( y ) ), 1 );
				r.add_plane( std::move( pl ) );
			}
		}
	}
	else
	{
		// hrm, we no longer have the URL :(
		throw_runtime( "Request for layer '{0}', view '{1}' does not exist in frame {2}",
					   layer, view, f.number() );
	}
	return r;
}

////////////////////////////////////////

std::shared_ptr<media::frame>
to_frame( const image_buf &i, const std::vector<std::string> &chans, const std::string &type, const media::metadata &meta )
{
	if ( i.size() < chans.size() )
		throw_runtime( "image does not have enough channels ({0}) for requested channel list size ({1})", i.size(), chans.size() );

	auto r = std::make_shared<media::frame>();
	// TODO: add layer / view names?
	r->register_layer( base::cstring() ).add_view( base::cstring() ).store(
		std::make_shared<frame_image_adapter>( i, chans, type, meta ) );
	return r;
#if 0
	engine::dimensions d = i.dims();
	std::shared_ptr<media::image_frame> r = std::make_shared<media::image_frame>( d.x1, d.y1, d.x2, d.y2 );
	for ( size_t c = 0, nC = chans.size(); c != nC; ++c )
	{
		const plane &p = i[c];
		media::image_buffer ib;
		if ( type == "f16" )
		{
			ib = media::image_buffer::simple_buffer<base::half>( p.x1(), p.y1(), p.x2(), p.y2() );
		}
		else if ( type == "u16" )
		{
			ib = media::image_buffer::simple_buffer<uint16_t>( p.x1(), p.y1(), p.x2(), p.y2() );
		}
		else
			throw_runtime( "Unknown/unhandled data type tag" );

		for ( int64_t y = p.y1(), ly = p.y2(); y <= ly; ++y )
		{
			const float *pL = p.line( static_cast<int>( y ) );
			ib.set_scanline( y, pL, 1 );
		}
		r->add_channel( chans[c], ib );
	}
	r->copy_meta( meta );
	return r;
#endif
}

////////////////////////////////////////

void
debug_save_image( const image_buf &i, const std::string &fn, int64_t sampNum, const std::vector<std::string> &chans, const std::string &type, const media::parameter_set &params, const media::metadata &meta )
{
	std::vector<media::track_description> tds;

	tds.push_back( media::TRACK_VIDEO );
	tds.back().rate( media::sample_rate( 1, 1 ) );
	tds.back().offset( sampNum );
	tds.back().duration( 1 );
	base::uri fnU( fn );
	if ( ! fnU )
		fnU.set_scheme( "file" );

	media::container oc = media::writer::open( fnU, tds, params );
	oc.video_tracks()[0]->store( sampNum, to_frame( i, chans, type, meta ), allocator::get() );
	std::cout << "Saved debug image to '" << fn << "', frame " << sampNum << std::endl;
}

////////////////////////////////////////

//image_buf
//load_frame( const std::shared_ptr<media::container> &c, size_t videoTrackIdx, int64_t sampNum )
//{
//	engine::dimensions d = nulldim;
//	auto vt = c->video_tracks()[videoTrackIdx];
//	d.x = vt->dimension_x();
//	d.y = vt->dimension_y();
//	d.z = vt->dimension_z();
//	d.w = vt->dimension_z();
//	return image_buf( "m.load_frame", d, c, videoTrackIdx, sampNum );
//}

////////////////////////////////////////

//engine::computed_value<bool>
//save_frame( const image_buf &i,
//			const std::vector<std::string> &chans,
//			const std::string &type,
//			const std::shared_ptr<media::container> &c,
//			size_t videoTrackIdx,
//			int64_t sampNum )
//{
//	engine::dimensions d = nulldim;
//	return engine::computed_value<bool>( "m.save_frame", d, i, chans, type, c, videoTrackIdx, sampNum );
//}

////////////////////////////////////////

void
add_media_io( engine::registry &r )
{
}

////////////////////////////////////////

} // image



