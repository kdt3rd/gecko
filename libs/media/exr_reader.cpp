// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "exr_reader.h"

#if defined(HAVE_OPENEXR)
# include "file_per_sample_reader.h"
# include "image.h"
# include "image_buffer.h"
# include "data.h"
# include "video_track.h"
# include "file_sequence.h"
# include <base/string_util.h>
# include <base/file_system.h>
# include <base/env.h>
# include <base/thread_util.h>
# include <color/standards.h>
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

# include <ImfBoxAttribute.h>
# include <ImfChannelListAttribute.h>
# include <ImfChromaticitiesAttribute.h>
# include <ImfCompressionAttribute.h>
# include <ImfDeepImageStateAttribute.h>
# include <ImfDoubleAttribute.h>
# include <ImfEnvmapAttribute.h>
# include <ImfFloatAttribute.h>
//# include <ImfFloatVectorAttribute.h>
# include <ImfIntAttribute.h>
# include <ImfKeyCodeAttribute.h>
# include <ImfLineOrderAttribute.h>
# include <ImfMatrixAttribute.h>
# include <ImfOpaqueAttribute.h>
# include <ImfPreviewImageAttribute.h>
# include <ImfRationalAttribute.h>
# include <ImfStringAttribute.h>
# include <ImfStringVectorAttribute.h>
# include <ImfTileDescriptionAttribute.h>
# include <ImfTimeCodeAttribute.h>
# include <ImfVecAttribute.h>

# include <ImfStandardAttributes.h>

# include <ImfPartType.h>

# include <ImfInputPart.h>
# include <ImfTiledInputPart.h>
# include <ImfDeepScanLineInputPart.h>
# include <ImfDeepTiledInputPart.h>

# include <ImfVersion.h>
# include <ImfChannelList.h>
# include <ImfMultiView.h>
# include <ImfThreading.h>
# pragma GCC diagnostic pop

# include <algorithm>
# include <iostream>
#endif

namespace media
{

#if defined(HAVE_OPENEXR)
namespace EXR = OPENEXR_IMF_NAMESPACE;
namespace IMATH = IMATH_NAMESPACE;

////////////////////////////////////////

namespace
{

class exr_istream : public EXR::IStream
{
public:
	exr_istream( base::istream &&str )
		: IStream( str.uri().c_str() ), _stream( std::move( str ) )
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

	EXR::Int64 tellg( void ) override
	{
		try
		{
			return static_cast<EXR::Int64>( _stream.tellg() );
		}
		catch ( ... )
		{
			std::throw_with_nested( std::runtime_error( "Error telling position on OpenEXR stream: " + _stream.uri() ) );
		}
	}

	void seekg( EXR::Int64 pos ) override
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
	base::istream _stream;
};

////////////////////////////////////////

plane_layout to_layout( const EXR::Channel &c )
{
	plane_layout pl;

	// openexr is always little endian
	pl._endian = base::endianness::LITTLE;
	pl._xsubsample = c.xSampling;
	pl._xsubsample_shift = c.xSampling - 1;
	pl._ysubsample = c.ySampling;
	pl._ysubsample_shift = c.ySampling - 1;

	switch ( c.type )
	{
		case EXR::UINT:
			pl._bits = 32;
			pl._floating = false;
			pl._unsigned = true;
			break;

		case EXR::HALF:
			pl._bits = 16;
			pl._floating = true;
			pl._unsigned = false;
			break;

		case EXR::FLOAT:
			pl._bits = 32;
			pl._floating = true;
			pl._unsigned = false;
			break;

		default:
			throw_runtime( "Unknown pixel type in EXR file" );
	}

	return pl;
}

template <typename M, typename V>
inline metadata_value create_mv( const V &x )
{
	return M::make( x );
}

template <typename M>
inline metadata_value create_mv( const EXR::LineOrder &l )
{
	switch ( l )
	{
		case EXR::LineOrder::INCREASING_Y: return M::make( 1 );
		case EXR::LineOrder::DECREASING_Y: return M::make( -1 );
		case EXR::LineOrder::RANDOM_Y: return M::make( 0 );

		default:
			break;
	}
	throw_logic( "Unknown EXR line order specification: {0}", static_cast<int>( l ) );
}

template <typename M>
inline metadata_value create_mv( const EXR::Compression &c )
{
	switch ( c )
	{
		case EXR::Compression::NO_COMPRESSION: return M::make( "none" );
		case EXR::Compression::RLE_COMPRESSION: return M::make( "rle" );
		case EXR::Compression::ZIPS_COMPRESSION: return M::make( "zips" );
		case EXR::Compression::ZIP_COMPRESSION: return M::make( "zip" );
		case EXR::Compression::PIZ_COMPRESSION: return M::make( "piz" );
		case EXR::Compression::PXR24_COMPRESSION: return M::make( "pxr24" );
		case EXR::Compression::B44_COMPRESSION: return M::make( "b44" );
		case EXR::Compression::B44A_COMPRESSION: return M::make( "b44a" );
		case EXR::Compression::DWAA_COMPRESSION: return M::make( "dwaa" );
		case EXR::Compression::DWAB_COMPRESSION: return M::make( "dwab" );

		default:
			break;
	}
	throw_logic( "Unknown EXR compression specification: {0}", static_cast<int>( c ) );
}

template <typename M>
inline metadata_value create_mv( const EXR::Envmap &e )
{
	switch ( e )
	{
		case EXR::Envmap::ENVMAP_LATLONG: return M::make( "latlong" );
		case EXR::Envmap::ENVMAP_CUBE: return M::make( "cube" );

		default:
			break;
	}
	throw_logic( "Unknown EXR environment mapping specification: {0}", static_cast<int>( e ) );
}

template <typename M, typename V>
inline metadata_value create_array_val( const V *p, size_t N )
{
	using T = typename M::value_type;
	T arr;
	precondition( N == arr.size(), "unexpected mismatch in array size and argument size" );
	for ( size_t i = 0; i != N; ++i )
		arr[i] = p[i];
	return metadata_value( M::tag, base::make_any<T>( arr ) );
}

template <typename M, typename V>
inline metadata_value create_mv( const IMATH_NAMESPACE::Matrix33<V> &mx )
{ return create_array_val<M>( mx.getValue(), 9 ); }

template <typename M, typename V>
inline metadata_value create_mv( const IMATH_NAMESPACE::Matrix44<V> &mx )
{ return create_array_val<M>( mx.getValue(), 16 ); }

template <typename M, typename V>
inline metadata_value create_mv( const IMATH_NAMESPACE::Vec2<V> &mx )
{ return create_array_val<M>( mx.getValue(), 2 ); }

template <typename M, typename V>
inline metadata_value create_mv( const IMATH_NAMESPACE::Vec3<V> &mx )
{ return create_array_val<M>( mx.getValue(), 3 ); }

template <typename M, typename V>
inline metadata_value create_mv( const IMATH_NAMESPACE::Box< IMATH_NAMESPACE::Vec2<V> > &b )
{ return create_array_val<M>( reinterpret_cast<const V *>( &b ), 4 ); }

template <typename M>
inline metadata_value create_mv( const EXR::Chromaticities &c )
{ return create_array_val<M>( reinterpret_cast<const float *>( &c ), 8 ); }

template <typename M>
inline metadata_value create_mv( const EXR::Rational &r )
{ return M::make( std::make_pair( r.n, r.d ) ); }

template <typename M>
inline metadata_value create_mv( const EXR::TimeCode &tc )
{
	uint32_t tflag = tc.timeAndFlags( EXR::TimeCode::TV60_PACKING );
	uint32_t user = tc.userData();
	return M::make( std::make_pair( tflag, user ) );
}

template <typename M>
inline metadata_value create_mv( const EXR::KeyCode &kc )
{
	int32_t vals[7] = {
		kc.filmMfcCode(),
		kc.filmType(),
		kc.prefix(),
		kc.count(),
		kc.perfOffset(),
		kc.perfsPerFrame(),
		kc.perfsPerCount() 
	};

	return create_array_val<M>( vals, 7 );
}

template <typename E, typename X, typename M>
inline void cast_and_set( M &md, const char *name, const EXR::Attribute &attr )
{
	const E *eAttr = dynamic_cast<const E *>( &attr );
	if ( eAttr )
		md.set_meta( name, create_mv<X>( eAttr->value() ) );
	else
		throw_runtime( "Unable to convert EXR attribute '{0}' to correct type (says '{1}')",
					   name, attr.typeName() );
}

////////////////////////////////////////

static inline media::sample_rate extract_rate( const EXR::Header &h )
{
	media::sample_rate sr;
	if ( EXR::hasFramesPerSecond( h ) )
	{
		const auto &fps = EXR::framesPerSecond( h );
		sr.set( fps.n, fps.d );
	}

	return sr;
}

////////////////////////////////////////

static inline bool isBuiltinAttr( base::cstring aname )
{
	return aname == "dataWindow" ||
		aname == "displayWindow" ||
		aname == "envmap" ||
		aname == "tiles" ||
		aname == "type";
}

template <typename M>
static void fill_metadata( M &md, color::state &cs, const EXR::Header &header )
{
	for ( auto attri = header.begin(); attri != header.end(); ++attri )
	{
		const char *attrname = attri.name();
		const EXR::Attribute &attrib = attri.attribute();
		base::cstring type{ attrib.typeName() };

		// we handle these specially or just flat ignore them (for now anyway)
		if ( type == EXR::ChannelListAttribute::staticTypeName() ||
			 type == EXR::PreviewImageAttribute::staticTypeName() ||
			 type == EXR::TileDescriptionAttribute::staticTypeName() )
		{
			continue;
		}

		base::cstring aname{ attrname };
		// other "built-in" attribute names we handle implicity
		if ( isBuiltinAttr( aname ) )
			continue;

		if ( type == EXR::CompressionAttribute::staticTypeName() )
			cast_and_set<EXR::CompressionAttribute, meta_string_t>( md, attrname, attrib );
		else if ( type == EXR::LineOrderAttribute::staticTypeName() )
			cast_and_set<EXR::LineOrderAttribute, meta_int32_t>( md, attrname, attrib );
		else if ( type == EXR::StringAttribute::staticTypeName() )
			cast_and_set<EXR::StringAttribute, meta_string_t>( md, attrname, attrib );
		else if ( type == EXR::FloatAttribute::staticTypeName() )
		{
			if ( aname == "whiteLuminance" )
			{
				const EXR::FloatAttribute *eAttr = dynamic_cast<const EXR::FloatAttribute *>( &attrib );
				if ( eAttr )
				{
					cs.luminance_scale( eAttr->value() );
					cs.rendered( false );
				}
			}
			else if ( aname == "pixelAspectRatio" )
			{
				const EXR::FloatAttribute *eAttr = dynamic_cast<const EXR::FloatAttribute *>( &attrib );
				if ( eAttr )
					md.aspect_ratio( eAttr->value() );
			}
			else
				cast_and_set<EXR::FloatAttribute, meta_float32_t>( md, attrname, attrib );
		}
		else if ( type == EXR::M33fAttribute::staticTypeName() )
			cast_and_set<EXR::M33fAttribute, meta_m33f_t>( md, attrname, attrib );
		else if ( type == EXR::M44fAttribute::staticTypeName() )
			cast_and_set<EXR::M44fAttribute, meta_m44f_t>( md, attrname, attrib );
		else if ( type == EXR::V3fAttribute::staticTypeName() )
			cast_and_set<EXR::V3fAttribute, meta_vec3f_t>( md, attrname, attrib );
		else if ( type == EXR::V3iAttribute::staticTypeName() )
			cast_and_set<EXR::V3iAttribute, meta_vec3i_t>( md, attrname, attrib );
		else if ( type == EXR::V2fAttribute::staticTypeName() )
			cast_and_set<EXR::V2fAttribute, meta_vec2f_t>( md, attrname, attrib );
		else if ( type == EXR::V2iAttribute::staticTypeName() )
			cast_and_set<EXR::V2iAttribute, meta_vec2i_t>( md, attrname, attrib );
		else if ( type == EXR::IntAttribute::staticTypeName() )
			cast_and_set<EXR::IntAttribute, meta_int32_t>( md, attrname, attrib );
		else if ( type == EXR::StringVectorAttribute::staticTypeName() )
			cast_and_set<EXR::StringVectorAttribute, meta_stringlist_t>( md, attrname, attrib );
		else if ( type == EXR::DoubleAttribute::staticTypeName() )
			cast_and_set<EXR::DoubleAttribute, meta_float64_t>( md, attrname, attrib );
		else if ( type == EXR::V2dAttribute::staticTypeName() )
			cast_and_set<EXR::V2dAttribute, meta_vec2d_t>( md, attrname, attrib );
		else if ( type == EXR::V3dAttribute::staticTypeName() )
			cast_and_set<EXR::V3dAttribute, meta_vec3d_t>( md, attrname, attrib );
		else if ( type == EXR::M33dAttribute::staticTypeName() )
			cast_and_set<EXR::M33dAttribute, meta_m33d_t>( md, attrname, attrib );
		else if ( type == EXR::M44dAttribute::staticTypeName() )
			cast_and_set<EXR::M44dAttribute, meta_m44d_t>( md, attrname, attrib );
		else if ( type == EXR::Box2iAttribute::staticTypeName() )
			cast_and_set<EXR::Box2iAttribute, meta_recti_t>( md, attrname, attrib );
		else if ( type == EXR::Box2fAttribute::staticTypeName() )
			cast_and_set<EXR::Box2fAttribute, meta_rectf_t>( md, attrname, attrib );
//			else if ( type == EXR::FloatVectorAttribute::staticTypeName() )
//				cast_and_set<EXR::FloatVectorAttribute, meta_float_list_t>( md, attrname, attrib );
		else if ( type == EXR::TimeCodeAttribute::staticTypeName() )
			cast_and_set<EXR::TimeCodeAttribute, meta_smpte_timecode_t>( md, attrname, attrib );
		else if ( type == EXR::KeyCodeAttribute::staticTypeName() )
			cast_and_set<EXR::KeyCodeAttribute, meta_keycode_t>( md, attrname, attrib );
		else if ( type == EXR::ChromaticitiesAttribute::staticTypeName() )
		{
			if ( aname == "chromaticities" )
			{
				const EXR::ChromaticitiesAttribute *eAttr = dynamic_cast<const EXR::ChromaticitiesAttribute *>( &attrib );
				if ( eAttr )
				{
					auto &cx = eAttr->value();
					cs.chroma( color::state::cx(
								   color::state::cx::xy( cx.red.x, cx.red.y ),
								   color::state::cx::xy( cx.green.x, cx.green.y ),
								   color::state::cx::xy( cx.blue.x, cx.blue.y ),
								   color::state::cx::xy( cx.white.x, cx.white.y ) ) );
				}
			}
			else
				cast_and_set<EXR::ChromaticitiesAttribute, meta_chromaticities_t>( md, attrname, attrib );
		}
		else if ( type == EXR::RationalAttribute::staticTypeName() )
			cast_and_set<EXR::RationalAttribute, meta_rational32_t>( md, attrname, attrib );
		else if ( type == EXR::EnvmapAttribute::staticTypeName() )
			cast_and_set<EXR::EnvmapAttribute, meta_string_t>( md, attrname, attrib );
		else
			std::cout << "EXR: Unknown attribute type " << type << ": " << attrname << std::endl;
	}
}

////////////////////////////////////////

class exr_image final : public image
{
public:
	exr_image(
		const std::shared_ptr<EXR::MultiPartInputFile> &f,
		const std::shared_ptr<exr_istream> &s,
		int part,
		const EXR::Header &header,
		std::vector<std::string> pnames,
		std::vector<std::string> pfullnames,
		const std::vector<plane_layout> &pl
			  )
		: _file( f ),
		  _stream( s ),
		  _header( header ),
		  _plane_names( std::move( pnames ) ),
		  _full_plane_names( std::move( pfullnames ) )
	{
		if ( header.type() == EXR::TILEDIMAGE )
			_tiled_part.reset( new EXR::TiledInputPart( *f, part ) );
		else
			_scan_part.reset( new EXR::InputPart( *f, part ) );

		const auto &disp = header.displayWindow();
		const auto &data = header.displayWindow();
		set_active_area( area_rect::from_points( data.min.x, data.min.y,
												 data.max.x, data.max.y ) );
		set_full_area( area_rect::from_points( disp.min.x, disp.min.y,
											   disp.max.x, disp.max.y ) );

		size_t nPs = _plane_names.size();
		for ( size_t cur = 0; cur != nPs; ++cur )
			register_plane( _plane_names[cur], pl[cur], 0.0 );

		// TODO: verify these are all appropriate, especially the
		// rendered state (right now, we set it to true if we find
		// whiteLuminance, otherwise assume it is display referred)
		color::state cs{
			color::space::RGB,
			color::make_standard<color::standard::BT_709>().chroma(),
			color::state::value_type( 1.0/0.18 ),
			color::state::value_type( 0 ),
			color::range::FULL,
			color::transfer::LINEAR,
			false
		};

		fill_metadata( *this, cs, header );
		color_state( cs );
	}

	~exr_image( void ) override
	{
		_tiled_part.reset();
		_scan_part.reset();
	}

protected:
	bool storage_interleaved( void ) const override
	{
		return false;
	}

	void fill_plane( size_t plane, image_buffer &buffer ) override
	{
		if ( plane >= _plane_names.size() )
			throw_runtime( "Attempt to access plane {0}, beyond the end of the EXR image ({1} planes)", plane, _plane_names.size() );

		const EXR::Channel &imfchan = _header.channels()[_full_plane_names[plane]];
		IMATH::Box2i dataWin = _header.dataWindow();
		// TODO: handle subsampling
		size_t bytes = 0;
		bool flt = false;

		switch ( imfchan.type )
		{
			case EXR::UINT:
				bytes = sizeof(uint32_t);
				break;

			case EXR::HALF:
				bytes = sizeof(base::half);
				flt = true;
				break;

			case EXR::FLOAT:
				bytes = sizeof(float);
				flt = true;
				break;
				
			case EXR::NUM_PIXELTYPES:
			default:
				throw_logic( "Unknown OpenEXR pixel type {0}", static_cast<int>( imfchan.type ) );
		}

		if ( buffer.is_floating() != flt || static_cast<size_t>( buffer.bits() ) != ( bytes * 8 ) )
			throw_runtime( "Attempt to access EXR image with wrong buffer type" );

		// TODO: support sub-scanline reading (double copy :()
		if ( buffer.x1() != dataWin.min.x || buffer.x2() != dataWin.max.x )
			throw_runtime( "Only full scanline reading of EXR implemented" );

		size_t scanlinebytes = static_cast<size_t>( dataWin.max.x - dataWin.min.x + 1 ) * bytes;
		char *data = static_cast<char *>( buffer.data() );
		data -= dataWin.min.x * bytes;
		data -= buffer.y1() * scanlinebytes;
		
		EXR::FrameBuffer fbuf;
		fbuf.insert(
			_full_plane_names[plane],
			EXR::Slice( imfchan.type, data, bytes, scanlinebytes ) );

		if ( _scan_part )
		{
			_scan_part->setFrameBuffer( fbuf );
			_scan_part->readPixels( buffer.y1(), buffer.y2() );
		}
		else if ( _tiled_part )
			throw_not_yet();
	}

	void fill_image( std::vector<image_buffer> &planes ) override
	{
		image_buffer::rect readarea;
		size_t pIdx = 0;
		EXR::FrameBuffer fbuf;

		for ( auto i = _header.channels().begin(); i != _header.channels().end(); ++i, ++pIdx )
		{
			// TODO: handle subsampling
			const EXR::Channel &curc = i.channel();
			EXR::PixelType type = curc.type;

			image_buffer &buffer = planes[pIdx];
			size_t bytes = 0;
			bool flt = false;
			switch ( type )
			{
				case EXR::UINT:
					bytes = sizeof(uint32_t);
					break;

				case EXR::HALF:
					bytes = sizeof(base::half);
					flt = true;
					break;

				case EXR::FLOAT:
					bytes = sizeof(float);
					flt = true;
					break;
				
				case EXR::NUM_PIXELTYPES:
				default:
					throw_logic( "Unknown OpenEXR pixel type {0}", static_cast<int>( type ) );
			}
			if ( pIdx == 0 )
				readarea = buffer.active_area();
			else if ( readarea != buffer.active_area() )
				throw_runtime( "Image reads must request the same region for all planes" );

			if ( buffer.is_floating() != flt )
				throw_runtime( "Buffer and file differ in floating point interpretation of data" );

			if ( static_cast<size_t>( buffer.bits() ) != bytes * 8 )
				throw_runtime( "Mismatch in bits for provided memory buffer and on-disk plane definition" );

			precondition( buffer.raw(), "Expect valid memory buffer to be provided to read" );

			char *data = static_cast<char *>( buffer.data() );
			data -= buffer.x1() * buffer.xstride_bytes();
			data -= buffer.y1() * buffer.ystride_bytes();

			fbuf.insert( _full_plane_names[pIdx],
						 EXR::Slice( type, data, buffer.xstride_bytes(), buffer.ystride_bytes() ) );
		}

		if ( _scan_part )
		{
			_scan_part->setFrameBuffer( fbuf );
			_scan_part->readPixels( readarea.y1(), readarea.y2() );
		}
		else if ( _tiled_part )
			throw_not_yet();
	}

	std::pair<int64_t, int64_t> compute_preferred_chunk( void ) const
	{
		// TODO:
		return image::compute_preferred_chunk();
	}

	std::shared_ptr<EXR::MultiPartInputFile> _file;
	std::shared_ptr<exr_istream> _stream;
	const EXR::Header &_header;
	std::unique_ptr<EXR::TiledInputPart> _tiled_part;
	std::unique_ptr<EXR::InputPart> _scan_part;
	std::vector<std::string> _plane_names;
	std::vector<std::string> _full_plane_names;
};

////////////////////////////////////////

class exr_deep final : public data
{
public:
	exr_deep(
		const std::shared_ptr<EXR::MultiPartInputFile> &f,
		const std::shared_ptr<exr_istream> &s,
		int part,
		const EXR::Header &header,
		std::vector<std::string> channames,
		std::vector<std::string> chanfullnames,
		const std::vector<plane_layout> &chanl
			  )
		: _file( f ),
		  _stream( s ),
		  _header( header ),
		  _chan_names( std::move( channames ) ),
		  _full_chan_names( std::move( chanfullnames ) )
	{
		if ( header.type() == EXR::DEEPTILE )
			_tiled_part.reset( new EXR::DeepTiledInputPart( *f, part ) );
		else
			_scan_part.reset( new EXR::DeepScanLineInputPart( *f, part ) );

		const auto &disp = header.displayWindow();
		const auto &data = header.displayWindow();
		set_active_area( area_rect( data.min.x, data.min.y,
									data.max.x - data.min.x + 1,
									data.max.y - data.min.y + 1 ) );
		set_full_area( area_rect( disp.min.x, disp.min.y,
								  disp.max.x - disp.min.x + 1,
								  disp.max.y - disp.min.y + 1 ) );

		size_t nChans = _chan_names.size();
		for ( size_t cur = 0; cur != nChans; ++cur )
			register_plane( _chan_names[cur] );//, chanl[cur] );

		// TODO: verify these are all appropriate...
		color::state cs{
			color::space::RGB,
			color::make_standard<color::standard::BT_709>().chroma(),
			color::state::value_type( 1.0/0.18 ),
			color::state::value_type( 0 ),
			color::range::FULL,
			color::transfer::LINEAR,
			false
		};

		fill_metadata( *this, cs, header );

		color_state( cs );
	}

	~exr_deep( void ) override
	{
		_tiled_part.reset();
		_scan_part.reset();
	}

protected:
	bool storage_interleaved( void ) const override
	{
		return false;
	}

	void fill_plane_line( size_t plane, int64_t y, std::vector<float> &buffer ) override
	{
		throw_not_yet();
	}

	std::pair<int64_t, int64_t> compute_preferred_chunk( void ) const
	{
		// TODO: base this on the compression present in the file
		return data::compute_preferred_chunk();
	}

	std::shared_ptr<EXR::MultiPartInputFile> _file;
	std::shared_ptr<exr_istream> _stream;
	const EXR::Header &_header;
	std::unique_ptr<EXR::DeepTiledInputPart> _tiled_part;
	std::unique_ptr<EXR::DeepScanLineInputPart> _scan_part;
	std::vector<std::string> _chan_names;
	std::vector<std::string> _full_chan_names;
};

////////////////////////////////////////

class exr_frame final : public frame
{
public:
	exr_frame( const std::shared_ptr<base::file_system> &fs, const base::uri &frm, int64_t num )
		: _uri( frm ), _stream( std::make_shared<exr_istream>( fs->open_read( frm ) ) )
	{
		_file = std::make_shared<EXR::MultiPartInputFile>( *_stream );
		int numP = _file->parts();
		// similar logic to ImfPartHelper.h but we want to create the image
		// or deep records at the same time...
		const EXR::StringVector *multiviews = nullptr;
		if ( numP == 1 )
		{
			const EXR::Header &header = _file->header( 0 );
			if ( EXR::hasMultiView( header ) )
				multiviews = &( multiView( header ) );
		}

		for ( int p = 0; p < numP; ++p )
		{
			const EXR::Header &header = _file->header( p );
			const EXR::ChannelList &chans = header.channels();

			std::vector<std::string> channames;
			std::vector<std::string> chanfullnames;
			std::vector<plane_layout> chanl;
			std::string viewname;
			if ( header.hasView() )
				viewname = header.view();

			layer &l = register_layer( header.hasName() ? header.name() : std::string() );
			for ( auto c = chans.begin(); c != chans.end(); ++c )
			{
				// TODO: this API will create multiple copies of the
				// string unless we make a temporary
				std::string fullname{ c.name() };
				std::string name;
				if ( multiviews )
				{
					std::string curview = EXR::viewFromChannelName( fullname, *multiviews );
					name = EXR::removeViewName( fullname, curview );
					if ( curview != viewname )
					{
						if ( ! channames.empty() )
						{
							view &v = l.add_view( viewname );
							if ( EXR::isImage( header.type() ) )
							{
								v.store(
									std::make_shared<exr_image>(
										_file, _stream, 0, header, channames, chanfullnames, chanl )
										);
							}
							else
							{
								v.store(
									std::make_shared<exr_deep>(
										_file, _stream, 0, header, channames, chanfullnames, chanl )
										);
							}
						}
						channames.clear();
						chanfullnames.clear();
						chanl.clear();
						viewname = curview;
					}
				}
				channames.emplace_back( name.empty() ? std::string( fullname ) : std::move( name ) );
				chanfullnames.emplace_back( std::move( fullname ) );
				chanl.emplace_back( to_layout( c.channel() ) );
			}

			if ( ! channames.empty() )
			{
				view &v = l.add_view( viewname );
				if ( EXR::isImage( header.type() ) )
				{
					v.store(
						std::make_shared<exr_image>(
							_file, _stream, 0, header, channames, chanfullnames, chanl )
							);
				}
				else
				{
					v.store(
						std::make_shared<exr_deep>(
							_file, _stream, 0, header, channames, chanfullnames, chanl )
							);
				}
			}
		}
	}

	sample_rate find_rate( void ) const
	{
		sample_rate sr;
		int numP = _file->parts();
		for ( int p = 0; p < numP; ++p )
		{
			const EXR::Header &header = _file->header( p );
			sr = extract_rate( header );
			if ( sr.valid() )
				break;
		}
		return sr;
	}

private:
	base::uri _uri;
	std::shared_ptr<exr_istream> _stream;
	std::shared_ptr<EXR::MultiPartInputFile> _file;
};

class exr_read_track final : public video_track
{
public:
	// file per frame, don't have the sample rate or track description (yet)...
	exr_read_track( file_sequence &&fseq, int64_t b, int64_t e )
		: video_track( std::string(), std::string(), b, e, sample_rate(), track_description( TRACK_VIDEO ) ),
		  _files( std::move( fseq ) )
	{
	}

	frame *doRead( int64_t f ) override
	{
		std::unique_ptr<exr_frame> ret{
			new exr_frame(
				base::file_system::get( _files.uri() ),
				_files.get_frame( f ),
				f )
		};

		if ( ! rate().valid() )
			update_rate( ret->find_rate() );

		return ret.release();
	}

	void doWrite( int64_t , const frame &, base::allocator & ) override
	{
		throw_logic( "reader asked to write a frame" );
	}

private:
	file_sequence _files;
};

////////////////////////////////////////

class OpenEXRReader final : public file_per_sample_reader
{
public:
	OpenEXRReader( void )
			: file_per_sample_reader( "OpenEXR" )
	{
		_description = "OpenEXR Reader";
		_extensions.emplace_back( "exr" );
		_extensions.emplace_back( "sxr" ); // stereo EXR (maybe)
		_extensions.emplace_back( "mxr" ); // multi-view EXR (maybe)
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
	~OpenEXRReader( void ) override = default;

	container create( const base::uri &u, const parameter_set &params ) override
	{
		container result;

		int64_t start, last;
		file_sequence fseq( u );
		auto fs = scan_samples( start, last, fseq );

		result.add_track( std::make_shared<exr_read_track>( std::move( fseq ), start, last ) );
		result.set_parameters( params );

		return result;
	}
};

} // empty namespace
#endif // HAVE_OPENEXR

////////////////////////////////////////

void register_exr_reader( void )
{
#ifdef HAVE_OPENEXR
	if ( EXR::globalThreadCount() == 0 )
		EXR::setGlobalThreadCount( static_cast<int>( base::thread::core_count() ) );
	
	reader::register_reader( std::make_shared<OpenEXRReader>() );
#endif
}

////////////////////////////////////////

}

