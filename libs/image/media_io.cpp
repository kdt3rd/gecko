//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "media_io.h"
#include <base/contract.h>
#include <media/writer.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

image_buf
extract_frame( const media::image_frame &f )
{
	std::vector<std::string> chans;
	chans.reserve( f.size() );
	for ( size_t i = 0, N = f.size(); i != N; ++i )
		chans.emplace_back( f.name( i ) );

	return extract_frame( f, chans );
}

////////////////////////////////////////

image_buf extract_frame( const media::image_frame &f, const std::vector<std::string> &chans )
{
	size_t nC = chans.size();
	if ( f.size() < nC )
		throw_runtime( "Request for {0} channels/planes, but media image only has {1}", nC, f.size() );

	image_buf r;

	int64_t dx1 = 0, dx2 = 0, dy1 = 0, dy2 = 0;
	for ( size_t i = 0, N = nC; i != N; ++i )
	{
		if ( ! f.has_channel( chans[i] ) )
			throw_runtime( "media image does not have channel '{0}'", chans[i] );
		const media::image_buffer &ib = f[chans[i]];
		if ( i == 0 )
		{
			dx1 = ib.x1();
			dx2 = ib.x2();
			dy1 = ib.y1();
			dy2 = ib.y2();
		}
		else if ( dx1 != ib.x1() || dx2 != ib.x2() )
			throw_runtime( "media image channel {0} width {1} does not match channel 0 width {2} ({3} - {4})", i, ib.width(), dx2 - dx1 + 1, dx1, dx2 );
		else if ( dy1 != ib.y1() || dy2 != ib.y2() )
			throw_runtime( "media image channel {0} height {1} does not match channel 0 width {2} ({3} - {4})", i, ib.height(), dy2 - dy1 + 1, dy1, dy2 );

		// TODO: do we want this?
		//r.add_plane( plane( "m.conv_to_plane", d, f, c[i] ) );
		// need to add hash functions, etc.
		plane p( dx1, dy1, dx2, dy2 );
		for ( int64_t y = dy1; y <= dy2; ++y )
			ib.get_scanline( y, p.line( static_cast<int>( y ) ), 1 );
		r.add_plane( std::move( p ) );
	}
	return r;
}

////////////////////////////////////////

std::shared_ptr<media::image_frame>
to_frame( const image_buf &i, const std::vector<std::string> &chans, const std::string &type, const media::metadata &meta )
{
	if ( i.size() < chans.size() )
		throw_runtime( "image does not have enough channels ({0}) for requested channel list size ({1})", i.size(), chans.size() );

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
}

////////////////////////////////////////

void
debug_save_image( const image_buf &i, const std::string &fn, int64_t sampNum, const std::vector<std::string> &chans, const std::string &type, const media::metadata &options, const media::metadata &meta )
{
	std::vector<media::track_description> tds;

	tds.push_back( media::TRACK_VIDEO );
	tds.back().rate( media::sample_rate( 1, 1 ) );
	tds.back().offset( sampNum );
	tds.back().duration( 1 );
	for ( auto &o: options )
		tds.back().set_option( o.first, o.second );

	base::uri fnU( fn );
	if ( ! fnU )
		fnU.set_scheme( "file" );

	media::container oc = media::writer::open( fnU, tds, options );
	oc.video_tracks()[0]->store( sampNum, to_frame( i, chans, type, meta ) );
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



