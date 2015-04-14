
#include "exr_reader.h"
#include "image_frame.h"
#include "video_track.h"
#include "file_sequence.h"
#include <base/string_util.h>
#include <base/file_system.h>

#include <ImfMultiPartInputFile.h>
#include <ImfInputPart.h>
#include <ImfVersion.h>
#include <ImfStringAttribute.h>
#include <ImfStringVectorAttribute.h>
#include <ImfChannelList.h>
#include <ImfMultiView.h>
#include <ImfStandardAttributes.h>

#include <algorithm>

namespace media
{

////////////////////////////////////////

namespace
{

class exr_istream : public Imf::IStream
{
public:
	exr_istream( std::istream &str )
		: IStream( "" ), _stream( str )
	{
	}

	bool read( char c[], int n ) override
	{
		return bool( _stream.read( c, n ) );
	}

	Imf::Int64 tellg( void ) override
	{
		return _stream.tellg();
	}

	void seekg( Imf::Int64 pos ) override
	{
		_stream.seekg( pos );
	}

	void clear( void ) override
	{
		_stream.clear();
	}

//	bool isMemoryMapped( void ) const override;

//	char *readMemoryMapped( int n ) override;

private:
	std::istream &_stream;
};


class exr_track : public video_track
{
public:
	exr_track( std::string n, int64_t b, int64_t e, const base::uri &files, size_t part, std::vector<std::string> &&chan )
		: video_track( std::move( n ), b, e ), _files( files ), _part( part ), _channels( std::move( chan ) )
	{
	}

	virtual std::shared_ptr<image_frame> at( int64_t f ) override
	{
		auto result = std::make_shared<image_frame>();

		auto fs = base::file_system::get( _files.uri() );
		std::shared_ptr<std::istream> stream = fs->open_read( _files.get_frame( f ) );
		exr_istream estr( *stream );
		Imf::MultiPartInputFile file( estr );

		Imf::InputPart input( file, _part );
		const Imf::Header header = input.header();

		Imath::Box2i disp = header.dataWindow();
		int64_t w = disp.max.x - disp.min.x + 1;
		int64_t h = disp.max.y - disp.min.y + 1;
		Imf::FrameBuffer fbuf;

		std::shared_ptr<base::half> buffer( new base::half[w*_channels.size()*h], base::array_deleter<base::half>() );
		int64_t xstride = sizeof(base::half) * 8 * _channels.size();
		int64_t ystride = xstride * w;

		for ( size_t c = 0; c < _channels.size(); ++c )
		{
			image_buffer imgbuf( buffer, w, h, xstride, ystride );
			imgbuf.set_offset( c * sizeof(base::half) * 8 );
			auto data = static_cast<base::half*>( imgbuf.data() ) - disp.min.x - disp.min.y * w;
			fbuf.insert( _channels.at( c ), Imf::Slice( Imf::HALF, reinterpret_cast<char*>( data ), sizeof(base::half) * _channels.size(), sizeof(base::half) * _channels.size() * w, 1, 1, 0.0 ) );
			result->add_channel( _channels.at( c ), imgbuf );
		}

		input.setFrameBuffer( fbuf );
		input.readPixels( disp.min.y, disp.max.y );

		return result;
	}

private:
	file_sequence _files;
	size_t _part;
	std::vector<std::string> _channels;
};

////////////////////////////////////////

void addexrtrack( container &c, const base::uri &u, int64_t start, int64_t last, std::string track, size_t part, const Imf::ChannelList &channels )
{
	std::vector<std::string> chans;
	for ( auto chan = channels.begin(); chan != channels.end(); ++chan )
		chans.emplace_back( chan.name() );
	std::reverse( chans.begin(), chans.end() );
	if ( !chans.empty() )
		c.add_track( std::make_shared<exr_track>( std::move( track ), start, last, u, part, std::move( chans ) ) );
}

}

////////////////////////////////////////

container exr_reader( const base::uri &u )
{
	container result;

	int64_t start = std::numeric_limits<int64_t>::max();
	int64_t last = std::numeric_limits<int64_t>::min();

	file_sequence fseq( u );
	auto fs = base::file_system::get( fseq.uri() );
	{
		auto dir = fs->readdir( u.parent() );
		while ( ++dir )
		{
			int64_t f = 0;
			if ( fseq.extract_frame( *dir, f ) )
			{
				start = std::min( start, f );
				last = std::max( last, f );
			}
		}
	}

	std::shared_ptr<std::istream> stream = fs->open_read( fseq.get_frame( start ) );
	exr_istream estr( *stream );

	Imf::MultiPartInputFile file( estr );
	if ( file.parts() == 1 )
	{
		const Imf::Header header = file.header( 0 );
		auto *mview = header.findTypedAttribute<Imf::StringVectorAttribute>( "multiView" );
		if ( Imf::hasMultiView( header ) )
		{
			const auto &mview = Imf::multiView( header );
			for ( auto &v: mview )
				addexrtrack( result, u, start, last, v, 0, Imf::channelsInView( v, header.channels(), mview ) );
			addexrtrack( result, u, start, last, std::string(), 0, Imf::channelsInNoView( header.channels(), mview ) );
		}
		else
		{
			if ( header.hasName() )
				addexrtrack( result, u, start, last, header.name(), 0, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, header.view(), 0, header.channels() );
			if ( !header.hasName() && !header.hasView() )
				addexrtrack( result, u, start, last, "default", 0, header.channels() );
		}
	}
	else
	{
		for ( size_t p = 0; p < file.parts(); ++p )
		{
			const Imf::Header header = file.header( p );
			if ( header.hasName() )
				addexrtrack( result, u, start, last, header.name(), p, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, header.view(), p, header.channels() );
		}
	}

	return result;
}

////////////////////////////////////////

}

