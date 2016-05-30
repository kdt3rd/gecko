//
// Copyright (c) 2016 Kimball Thurston & Ian Godin
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

#include "exr_reader.h"

#if defined(HAVE_OPENEXR)
# include "file_per_sample_reader.h"
# include "image_frame.h"
# include "video_track.h"
# include "file_sequence.h"
# include <base/string_util.h>
# include <base/file_system.h>
# include <base/env.h>
# include <base/thread_util.h>
# include <thread>

# pragma GCC diagnostic push
# if defined(__clang__)
#  pragma GCC diagnostic ignored "-Wreserved-id-macro"
#  pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#  pragma GCC diagnostic ignored "-Wdeprecated-register"
#  pragma GCC diagnostic ignored "-Wweak-vtables"
# endif
# pragma GCC diagnostic ignored "-Wdeprecated"
# pragma GCC diagnostic ignored "-Wconversion"
# pragma GCC diagnostic ignored "-Wfloat-equal"
# pragma GCC diagnostic ignored "-Wshadow"
# pragma GCC diagnostic ignored "-Wsign-conversion"
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wunused-parameter"

# include <ImfMultiPartInputFile.h>
# include <ImfInputPart.h>
# include <ImfVersion.h>
# include <ImfStringAttribute.h>
# include <ImfStringVectorAttribute.h>
# include <ImfChannelList.h>
# include <ImfMultiView.h>
# include <ImfStandardAttributes.h>
# include <ImfThreading.h>
# pragma GCC diagnostic pop

# include <algorithm>
# include <iostream>
#endif

namespace media
{

////////////////////////////////////////

#if defined(HAVE_OPENEXR)
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
			if ( _stream.read( c, n ) )
			{
				if ( static_cast<std::streamsize>( n ) == _stream.gcount() )
					return true;

				if ( _stream.eof() )
					return false;
			}
			else if ( _stream.eof() )
				return false;
			throw std::runtime_error( "Stream failed during read" );
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


class exr_read_track : public video_track
{
public:
	exr_read_track( std::string n, int64_t b, int64_t e, const sample_rate &sr, const base::uri &files, int part, std::vector<std::string> &&chan )
		: video_track( std::move( n ), b, e, sr, media::track_description( media::TRACK_VIDEO ) ), _files( files ), _part( part ), _channels( std::move( chan ) )
	{
	}

	virtual image_frame *doRead( int64_t f )
	{
		std::unique_ptr<image_frame> ret;

		auto fs = base::file_system::get( _files.uri() );
		base::istream stream = fs->open_read( _files.get_frame( f ) );
		exr_istream estr( stream );
		int numThreads = Imf::globalThreadCount();
		// assuming pipelining, we want to share threads with the writing, right?
		numThreads = ( numThreads + 1 ) / 2;

		Imf::MultiPartInputFile file( estr, numThreads );

		Imf::InputPart input( file, _part );
		const Imf::Header header = input.header();

		Imath::Box2i disp = header.dataWindow();
		int64_t w = disp.max.x - disp.min.x + 1;
		int64_t h = disp.max.y - disp.min.y + 1;
		Imf::FrameBuffer fbuf;

		ret.reset( new image_frame( w, h ) );
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
			ret->add_channel( cname, imgbuf );
		}

		input.setFrameBuffer( fbuf );
		input.readPixels( disp.min.y, disp.max.y );

		return ret.release();
	}

	virtual void doWrite( int64_t , const image_frame & )
	{
		throw_logic( "reader asked to write a frame" );
	}
private:
	file_sequence _files;
	int _part;
	std::vector<std::string> _channels;
};

////////////////////////////////////////

void addexrtrack( container &c, const base::uri &u, int64_t start, int64_t last, const media::sample_rate &sr, std::string track, int part, const Imf::ChannelList &channels )
{
	std::vector<std::string> chans;
	for ( auto chan = channels.begin(); chan != channels.end(); ++chan )
		chans.emplace_back( chan.name() );
	std::reverse( chans.begin(), chans.end() );
	if ( !chans.empty() )
		c.add_track( std::make_shared<exr_read_track>( std::move( track ), start, last, sr, u, part, std::move( chans ) ) );
}

media::sample_rate extract_rate( const Imf::Header &h )
{
	media::sample_rate sr;
	if ( Imf::hasFramesPerSecond( h ) )
	{
		const auto &fps = Imf::framesPerSecond( h );
		sr.set( fps.n, fps.d );
	}
	else
		sr.set( 24, 1 );
	return sr;
}

////////////////////////////////////////

class OpenEXRReader : public file_per_sample_reader
{
public:
	OpenEXRReader( void )
			: file_per_sample_reader( "OpenEXR" )
	{
		_description = "OpenEXR Reader";
		_extensions.emplace_back( "exr" );
		_extensions.emplace_back( "sxr" ); // stereo EXR
		_extensions.emplace_back( "mxr" ); // multi-view EXR
		_extensions.emplace_back( "aces" );
		// The multi-view document requests paying attention to an environment variable
		std::string val = base::env::global().get( "MULTIVIEW_EXR_EXT" );
		if ( ! val.empty() )
		{
			val = base::to_lower( val );
			if ( val != "exr" && val != "sxr" && val != "mxr" && val != "aces" )
				_extensions.emplace_back( std::move( val ) );
		}
		std::vector<uint8_t> m{0x76, 0x2f, 0x31, 0x01};
		_magics.emplace_back( std::move( m ) );
	}
	virtual ~OpenEXRReader( void ) = default;

	virtual container create( const base::uri &u, const metadata &params );
};

container
OpenEXRReader::create( const base::uri &u, const metadata & )
{
	container result;

	int64_t start, last;
	file_sequence fseq( u );
	auto fs = scan_samples( start, last, fseq );
	base::istream stream = fs->open_read( fseq.get_frame( start ) );
	exr_istream estr( stream );

	Imf::MultiPartInputFile file( estr );
	if ( file.parts() == 1 )
	{
		const Imf::Header header = file.header( 0 );
		sample_rate sr = extract_rate( header );

		if ( Imf::hasMultiView( header ) )
		{
			const auto &mview = Imf::multiView( header );
			for ( auto &v: mview )
				addexrtrack( result, u, start, last, sr, v, 0, Imf::channelsInView( v, header.channels(), mview ) );
			addexrtrack( result, u, start, last, sr, std::string(), 0, Imf::channelsInNoView( header.channels(), mview ) );
		}
		else
		{
			if ( header.hasName() )
				addexrtrack( result, u, start, last, sr, header.name(), 0, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, sr, header.view(), 0, header.channels() );
			if ( !header.hasName() && !header.hasView() )
				addexrtrack( result, u, start, last, sr, "default", 0, header.channels() );
		}
	}
	else
	{
		for ( int p = 0; p < file.parts(); ++p )
		{
			const Imf::Header header = file.header( p );
			sample_rate sr = extract_rate( header );
			if ( header.hasName() )
				addexrtrack( result, u, start, last, sr, header.name(), p, header.channels() );
			if ( header.hasView() )
				addexrtrack( result, u, start, last, sr, header.view(), p, header.channels() );
		}
	}

	return result;
}

} // empty namespace
#endif // HAVE_OPENEXR

////////////////////////////////////////

void register_exr_reader( void )
{
#ifdef HAVE_OPENEXR
	if ( Imf::globalThreadCount() == 0 )
		Imf::setGlobalThreadCount( static_cast<int>( base::thread::core_count() ) );
	
	reader::register_reader( std::make_shared<OpenEXRReader>() );
#endif
}

////////////////////////////////////////

}

