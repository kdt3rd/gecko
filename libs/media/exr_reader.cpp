
#include "exr_reader.h"
//#include "numbered_files.h"
#include "image_frame.h"
#include "video_track.h"
#include <base/string_util.h>

#include <ImfMultiPartInputFile.h>
#include <ImfVersion.h>
#include <ImfStringAttribute.h>
#include <ImfStringVectorAttribute.h>
#include <ImfChannelList.h>
#include <ImfMultiView.h>
#include <ImfStandardAttributes.h>

namespace media
{

////////////////////////////////////////

namespace detail
{

class exr_track : public video_track
{
public:
	exr_track( std::string n, int64_t b, int64_t e, const base::uri &files, size_t part, std::vector<std::string> &&chan )
		: video_track( std::move( n ), b, e ), _uri( files ), _part( part ), _channels( std::move( chan ) )
	{
	}

	virtual std::shared_ptr<image_frame> at( int64_t f ) override
	{
		auto result = std::make_shared<image_frame>();

		std::shared_ptr<std::istream> stream = base::open_uri( uri_for_frame( _uri, f ) );
		exr_istream estr( *stream );
		Imf::MultiPartInputFile file( estr );

		Imf::InputPart input( file, _part );
		const Imf::Header header = input.header( p );

		Imf::Box2i disp = header.dataWindow();
		int64_t w = disp.max.x - disp.min.x + 1;
		int64_t h = disp.max.y - disp.min.y + 1;
		Imf::FrameBuffer fbuf;

		for ( auto &c: _channels )
		{
			image_buffer imgbuf( w, h );
			char *data = static_cast<char*>( imgbuf.data() - dw.min.x - dw.min.y * w );
			fbuf.insert( c, Imf::Slice( Imf::HALF, data, sizeof( uin16_t ), sizeof( uin16_t ) * w, 1, 1, 0.0 ) );
			result->add_channel( c, imgbuf );
		}

		input.setFrameBuffer( fbuf );
		input.readPixels( disp.min.y, disp.max.y );
	}

private:
	base::uri _uri;
	size_t _part;
	std::vector<std::string> _channels;
};

}

////////////////////////////////////////

void addexrtrack( container &c, const base::uri &u, int64_t start, int64_t last, std::string &&track, size_t part, const Imf::ChannelList &channels )
{
	std::vector<std::string> chans;
	for ( auto chan = channels.begin(); chan != channels.end(); ++chan )
		chans.emplace_back( chan.name() );
	if ( !chans.empty() )
		c.add_track( std::move( track ), std::make_shared<exr_track>( track, start, last, u, part, chans ) );
}

////////////////////////////////////////

container exr_reader( base::uri &u )
{
	container result;

	int64_t start = 0;
	int64_t last = 0;

	Imf::MultiPartInputFile file( ... );
	if ( file.parts() == 1 )
	{
		const Imf::Header header = file.header( 0 );
		auto *mview = header.findTypedAttribute<Imf::StringVectorAttribute>( "multiView" );
		if ( Imf::hasMultiView( header ) )
		{
			const auto &mview = Imf::multiView( header );
			for ( auto &v: mview )
				addexrtrack( result, u, start, last, std::string( v ), 0, Imf::channelsInView( v, header.channels(), mview ) );
			addexrtrack( result, u, start, last, std::string(), 0, Imf::channelsInNoView( header.channels(), mview ) );
		}
		else
		{
			if ( header.hasName() )
				addexrtrack( result, u, start, last, std::string( header.name() ), 0, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, std::string( header.view() ), 0, header.channels() );
		}
	}
	else
	{
		for ( size_t p = 0; p < file.parts(); ++p )
		{
			const Imf::Header header = file.header( p );
			if ( header.hasName() )
				addexrtrack( result, u, start, last, std::string( header.name() ), p, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, std::string( header.view() ), p, header.channels() );
		}
	}
}

////////////////////////////////////////

}

