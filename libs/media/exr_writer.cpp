//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "exr_writer.h"
#if defined(HAVE_OPENEXR)
# include "writer.h"
# include "image_frame.h"
# include "video_track.h"
# include "file_sequence.h"
# include "track_description.h"
# include <base/contract.h>
# include <base/string_util.h>
# include <base/thread_util.h>
# include <base/file_system.h>
# include <color/state.h>
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

#ifdef HAVE_OPENEXR
namespace
{

using namespace media;

class exr_ostream : public Imf::OStream
{
public:
	exr_ostream( base::ostream &str )
			: OStream( str.uri().c_str() ), _stream( str )
	{
	}
	virtual ~exr_ostream( void ) = default;

	virtual void write( const char c[], int n ) override
	{
		try
		{
			if ( ! _stream.write( c, n ) )
				throw std::runtime_error( "Stream failed during write" );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error writing to OpenEXR stream: " + _stream.uri() ) );
		}
	}

	Imf::Int64 tellp( void ) override
	{
		try
		{
			return static_cast<Imf::Int64>( _stream.tellp() );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error telling position on OpenEXR output stream: " + _stream.uri() ) );
		}
	}

	void seekp( Imf::Int64 pos ) override
	{
		try
		{
			_stream.seekp( static_cast<std::streamoff>( pos ) );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error seeking to position on OpenEXR output stream: " + _stream.uri() ) );
		}
	}

private:
	base::ostream &_stream;
};

class exr_write_track : public video_track
{
public:
	exr_write_track( std::string n, std::string v, int64_t b, int64_t e, const sample_rate &sr, const base::uri &files, const media::track_description &td, const metadata &parms )
			: video_track( std::move( n ), std::move( v ), b, e, sr, td ),
			  _files( files ), _compression( Imf::NO_COMPRESSION )
	{
		using namespace base;

		auto c = parms.find( "compression" );
		if ( c != parms.end() )
		{
			const std::string &comp = any_cast<const std::string &>( c->second );
			if ( comp == "none" )
				_compression = Imf::NO_COMPRESSION;
			else if ( comp == "rle" )
				_compression = Imf::RLE_COMPRESSION;
			else if ( comp == "zips" )
				_compression = Imf::ZIPS_COMPRESSION;
			else if ( comp == "zip" )
				_compression = Imf::ZIP_COMPRESSION;
			else if ( comp == "piz" )
				_compression = Imf::PIZ_COMPRESSION;
			else if ( comp == "pxr24" )
				_compression = Imf::PXR24_COMPRESSION;
			else if ( comp == "b44" )
				_compression = Imf::B44_COMPRESSION;
			else if ( comp == "b44a" )
				_compression = Imf::RLE_COMPRESSION;
			else if ( comp == "dwaa" )
				_compression = Imf::DWAA_COMPRESSION;
			else if ( comp == "dwab" )
				_compression = Imf::DWAB_COMPRESSION;
			else
				throw_runtime( "Unknown compression scheme {0} given to OpenEXR", comp );
		}

		auto id = parms.find( media_ImageDescription );
		if ( id != parms.end() )
		{
			const color::state &st = any_cast<const color::state &>( id->second );
			const color::state::cx &chroma = st.chroma();
			_chroma = Imf::Chromaticities(
				IMATH_NAMESPACE::V2f( float(chroma.red.x), float(chroma.red.y) ),
				IMATH_NAMESPACE::V2f( float(chroma.green.x), float(chroma.green.y) ),
				IMATH_NAMESPACE::V2f( float(chroma.blue.x), float(chroma.blue.y) ),
				IMATH_NAMESPACE::V2f( float(chroma.white.x), float(chroma.white.y) ) );
		}
		else
		{
			color::chromaticities<float> r709 = color::two_deg::Rec_709<float>();
			_chroma = Imf::Chromaticities(
				IMATH_NAMESPACE::V2f( float(r709.red.x), float(r709.red.y) ),
				IMATH_NAMESPACE::V2f( float(r709.green.x), float(r709.green.y) ),
				IMATH_NAMESPACE::V2f( float(r709.blue.x), float(r709.blue.y) ),
				IMATH_NAMESPACE::V2f( float(r709.white.x), float(r709.white.y) ) );
			
		}

		auto idw = parms.find( media_ImageDisplayWin );
		if ( idw != parms.end() )
			_disp_win = any_cast<Imath::Box2i>( id->second );
	}

	virtual image_frame *doRead( int64_t )
	{
		throw_logic( "writer asked to read a frame" );
	}

	virtual void doWrite( int64_t f, const image_frame &frm )
	{
		auto fs = base::file_system::get( _files.uri() );
		base::ostream stream = fs->open_write( _files.get_frame( f ) );
		exr_ostream estr( stream );

		float pAR = 1.F;
		Imath::Box2i dataWin;
		dataWin.min.x = frm.x1();
		dataWin.min.y = frm.y1();
		dataWin.max.x = frm.x2();
		dataWin.max.y = frm.y2();

		// TODO: extract other metadata to write
		Imath::Box2i dispWin = _disp_win;
		if ( dispWin.min.x > dispWin.max.x )
			dispWin = dataWin;

		// TODO: make these NOT metadata but implicit in the
		// definition!  then we can just loop over the metadata and
		// insert it into the header if we understand the type...
		using namespace base;
		auto idw = frm.meta().find( media_ImageDisplayWin );
		if ( idw != frm.meta().end() )
			dispWin = any_cast<Imath::Box2i>( idw->second );
		auto iAR = frm.meta().find( media_ImagePixelAspectRatio );
		if ( iAR != frm.meta().end() )
			pAR = any_cast<float>( iAR->second );

//		std::cout << "writing display window of " << dispWin.min.x << ',' << dispWin.min.y << " - " << dispWin.max.x << ',' << dispWin.max.y << std::endl;
		Imf::Header fHeader( dispWin, dataWin, pAR, IMATH_NAMESPACE::V2f(0, 0), 1, Imf::INCREASING_Y, _compression );

		fHeader.insert( Imf::ChromaticitiesAttribute::staticTypeName(), Imf::ChromaticitiesAttribute( _chroma ) );
		for ( const auto &rd: frm.render_data() )
			fHeader.insert( rd.first.c_str(), Imf::StringAttribute( any_cast<const std::string &>( rd.second ) ) );

		for ( size_t c = 0; c < frm.size(); ++c )
		{
			const image_buffer &ib = frm.at( c );
			if ( ib.bits() == 16 && ib.is_floating() )
				fHeader.channels().insert( frm.name( c ), Imf::Channel( Imf::HALF ) );
			else if ( ib.bits() == 32 && ib.is_floating() )
				fHeader.channels().insert( frm.name( c ), Imf::Channel( Imf::FLOAT ) );
			else if ( ib.bits() == 32 )
				fHeader.channels().insert( frm.name( c ), Imf::Channel( Imf::UINT ) );
			else
				throw_not_yet();
		}

		// TODO: how do we do multi-part output???  need to re-do
		// sample / track description to have all the samples (images)
		// for a frame as one thing, and can't write them
		// separately...
		Imf::OutputFile file( estr, fHeader );
		Imf::FrameBuffer fB;
		for ( size_t c = 0; c < frm.size(); ++c )
		{
			const image_buffer &ib = frm.at( c );
			int64_t w = ib.width();
			if ( ib.bits() == 16 && ib.is_floating() )
			{
				char *data = ( const_cast<char *>( static_cast<const char*>( ib.data() ) )
							   - static_cast<int64_t>( dataWin.min.x + dataWin.min.y * w ) * sizeof(base::half) );
				fB.insert( frm.name( c ),
						   Imf::Slice( Imf::HALF,
									   data,
									   static_cast<size_t>( ib.xstride_bytes() ),
									   static_cast<size_t>( ib.ystride_bytes() ) ) );
			}
			else if ( ib.bits() == 32 && ib.is_floating() )
			{
				char *data = ( const_cast<char *>( static_cast<const char*>( ib.data() ) )
							   - static_cast<int64_t>( dataWin.min.x + dataWin.min.y * w ) * sizeof(float) );
				fB.insert( frm.name( c ),
						   Imf::Slice( Imf::FLOAT,
									   data,
									   static_cast<size_t>( ib.xstride_bytes() ),
									   static_cast<size_t>( ib.ystride_bytes() ) ) );
			}
			else if ( ib.bits() == 32 )
			{
				char *data = ( const_cast<char *>( static_cast<const char*>( ib.data() ) )
							   - static_cast<int64_t>( dataWin.min.x + dataWin.min.y * w ) * sizeof(uint32_t) );
				fB.insert( frm.name( c ),
						   Imf::Slice( Imf::UINT,
									   data,
									   static_cast<size_t>( ib.xstride_bytes() ),
									   static_cast<size_t>( ib.ystride_bytes() ) ) );
			}
		}

		file.setFrameBuffer( fB );
		file.writePixels( static_cast<int>( frm.height() ) );
	}

	virtual void doWrite( int64_t f, const std::vector<std::shared_ptr<image_frame>> &frms )
	{
	}


private:
	file_sequence _files;
	Imf::Compression _compression;
	Imf::Chromaticities _chroma;
	Imath::Box2i _disp_win;
};

////////////////////////////////////////

class OpenEXRWriter : public writer
{
public:
	OpenEXRWriter( void )
		: writer( "OpenEXR" )
	{
		_description = "OpenEXR Writer";
		_extensions.emplace_back( "exr" );
		_extensions.emplace_back( "sxr" );
		_extensions.emplace_back( "mxr" ); // multi-view EXR
		_extensions.emplace_back( "aces" );
		_parms.push_back( media::parameter_definition(
							  "compression", 
							  std::vector<std::string>{
								  "none",
								  "rle",
								  "zips",
								  "zip",
								  "piz"
								  "pxr24",
								  "b44",
								  "b44a",
								  "dwaa",
								  "dwab"
									  } ) );
		_parms.back().help( "Set the compression style:\n"
							" none   - No compression\n"
							" zips   - Zip, one scaneline per zip chunk\n"
							" zip    - Zip, 16 scanlines per chunk\n"
							" piz    - Piz-based wavelet compression\n"
							" pxr24  - Lossy 24-bit float compression\n"
							" b44    - Lossy 4-by-4 compression\n"
							" b44a   - Lossy 4-by-4, flat fields are compressed more\n"
							" dwaa   - Lossy DCT in blocks of 32 scanlines\n"
							" dwab   - Lossy DCT in blocks of 256 scanlines\n"
							);
	}
	virtual ~OpenEXRWriter( void ) = default;

	virtual container create( const base::uri &u, const std::vector<track_description> &td, const metadata &params );
};

container
OpenEXRWriter::create( const base::uri &u, const std::vector<track_description> &tdlist, const metadata &params )
{
	container ret;

	for ( auto &td: tdlist )
	{
		if ( td.type() != TRACK_VIDEO )
			throw_runtime( "OpenEXR only supports video tracks right now" );

		ret.add_track( std::make_shared<exr_write_track>( td.name(), td.view(), td.offset(), td.offset() + td.duration() - 1, td.rate(), u, td, params ) );
	}
	return ret;
}


}
#endif


////////////////////////////////////////


namespace media
{

void
register_exr_writer( void )
{
#ifdef HAVE_OPENEXR
	if ( Imf::globalThreadCount() == 0 )
		Imf::setGlobalThreadCount( static_cast<int>( base::thread::core_count() ) );

	writer::register_writer( std::make_shared<OpenEXRWriter>() );
#endif
}

////////////////////////////////////////

} // media



