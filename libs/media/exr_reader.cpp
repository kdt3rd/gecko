
#include "exr_reader.h"
#include "image_frame.h"
#include "video_track.h"
#include "file_sequence.h"
#include <base/string_util.h>
#include <base/file_system.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wweak-vtables"
#include <ImfMultiPartInputFile.h>
#include <ImfInputPart.h>
#include <ImfVersion.h>
#include <ImfStringAttribute.h>
#include <ImfStringVectorAttribute.h>
#include <ImfChannelList.h>
#include <ImfMultiView.h>
#include <ImfStandardAttributes.h>
#pragma GCC diagnostic pop

#include <algorithm>
#include <iostream>

namespace media
{

////////////////////////////////////////

namespace
{

class exr_istream : public Imf::IStream
{
public:
	exr_istream( base::istream &str )
			: IStream( str.uri().c_str() ), _stream( str )
	{
	}

	bool read( char c[], int n ) override
	{
		try
		{
			return bool( _stream.read( c, n ) );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error reading from OpenEXR stream: " + _stream.uri() ) );
		}
	}

	Imf::Int64 tellg( void ) override
	{
		try
		{
			return static_cast<Imf::Int64>( _stream.tellg() );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error telling position on OpenEXR stream: " + _stream.uri() ) );
		}
	}

	void seekg( Imf::Int64 pos ) override
	{
		try
		{
			_stream.seekg( static_cast<std::streamoff>( pos ) );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error seeking to position on OpenEXR stream: " + _stream.uri() ) );
		}
	}

	void clear( void ) override
	{
		try
		{
			_stream.clear();
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error clearing on OpenEXR stream: " + _stream.uri() ) );
		}
	}

//	bool isMemoryMapped( void ) const override;

//	char *readMemoryMapped( int n ) override;

private:
	base::istream &_stream;
};


class exr_track : public video_track
{
public:
	exr_track( std::string n, int64_t b, int64_t e, const base::uri &files, int part, std::vector<std::string> &&chan )
		: video_track( std::move( n ), b, e ), _files( files ), _part( part ), _channels( std::move( chan ) )
	{
	}

	virtual std::shared_ptr<image_frame> at( int64_t f ) override
	{
		auto fs = base::file_system::get( _files.uri() );
		base::istream stream = fs->open_read( _files.get_frame( f ) );
		exr_istream estr( stream );
		Imf::MultiPartInputFile file( estr );

		Imf::InputPart input( file, _part );
		const Imf::Header header = input.header();

		Imath::Box2i disp = header.dataWindow();
		int64_t w = disp.max.x - disp.min.x + 1;
		int64_t h = disp.max.y - disp.min.y + 1;
		Imf::FrameBuffer fbuf;

		auto result = std::make_shared<image_frame>( w, h );

		for ( size_t c = 0; c < _channels.size(); ++c )
		{
			const std::string &cname = _channels.at( c );
			const Imf::Channel &imfchan = header.channels()[cname];
			image_buffer imgbuf;
			size_t bytes = 0;
			switch ( imfchan.type )
			{
				case Imf::UINT:
					bytes = sizeof(uint32_t);
					imgbuf = image_buffer::simple_buffer<uint32_t>( w, h );
					break;

				case Imf::HALF:
					bytes = sizeof(base::half);
					imgbuf = image_buffer::simple_buffer<base::half>( w, h );
					break;

				case Imf::FLOAT:
					bytes = sizeof(float);
					imgbuf = image_buffer::simple_buffer<float>( w, h );
					break;

				case Imf::NUM_PIXELTYPES:
//				default:
					throw_logic( "unknown OpenEXR pixel type {0}", static_cast<int>( imfchan.type ) );
			}

			char *data = static_cast<char*>( imgbuf.data() ) - static_cast<size_t>( disp.min.x + disp.min.y * w ) * bytes;
			fbuf.insert( cname, Imf::Slice( imfchan.type, data, bytes, bytes * static_cast<size_t>(w), 1, 1, 0.0 ) );
			result->add_channel( cname, imgbuf );
		}

		input.setFrameBuffer( fbuf );
		input.readPixels( disp.min.y, disp.max.y );

		return result;
	}

private:
	file_sequence _files;
	int _part;
	std::vector<std::string> _channels;
};

////////////////////////////////////////

void addexrtrack( container &c, const base::uri &u, int64_t start, int64_t last, std::string track, int part, const Imf::ChannelList &channels )
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

	base::istream stream = fs->open_read( fseq.get_frame( start ) );
	exr_istream estr( stream );

	Imf::MultiPartInputFile file( estr );
	if ( file.parts() == 1 )
	{
		const Imf::Header header = file.header( 0 );
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
		for ( int p = 0; p < file.parts(); ++p )
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

