// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "exr_writer.h"
#if defined( HAVE_OPENEXR )
#    include "file_sequence.h"
#    include "image.h"
#    include "image_buffer.h"
#    include "track_description.h"
#    include "video_track.h"
#    include "writer.h"

#    include <base/contract.h>
#    include <base/file_system.h>
#    include <base/string_util.h>
#    include <base/thread_util.h>
#    include <base/time_util.h>
#    include <chrono>
#    include <color/state.h>
#    include <iomanip>
#    include <sstream>
#    include <string.h>
#    include <thread>
#    include <time.h>

#    pragma GCC diagnostic push
#    if defined( __clang__ )
#        pragma GCC diagnostic ignored "-Wreserved-id-macro"
#        pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#        pragma GCC diagnostic ignored "-Wdeprecated-register"
#        pragma GCC diagnostic ignored "-Wweak-vtables"
#    endif
#    pragma GCC diagnostic ignored "-Wdeprecated"
#    pragma GCC diagnostic ignored "-Wconversion"
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#    pragma GCC diagnostic ignored "-Wshadow"
#    pragma GCC diagnostic ignored "-Wsign-conversion"
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Wunused-parameter"

#    include <ImfBoxAttribute.h>
#    include <ImfChannelList.h>
#    include <ImfChannelListAttribute.h>
#    include <ImfChromaticitiesAttribute.h>
#    include <ImfCompressionAttribute.h>
#    include <ImfDeepImageStateAttribute.h>
#    include <ImfDoubleAttribute.h>
#    include <ImfEnvmapAttribute.h>
#    include <ImfFloatAttribute.h>
#    include <ImfMultiPartOutputFile.h>
#    include <ImfMultiView.h>
#    include <ImfOutputPart.h>
#    include <ImfPartType.h>
#    include <ImfThreading.h>
#    include <ImfTiledOutputPart.h>
#    include <ImfVersion.h>
//# include <ImfFloatVectorAttribute.h>
#    include <ImfIntAttribute.h>
#    include <ImfKeyCodeAttribute.h>
#    include <ImfLineOrderAttribute.h>
#    include <ImfMatrixAttribute.h>
#    include <ImfOpaqueAttribute.h>
#    include <ImfPreviewImageAttribute.h>
#    include <ImfRationalAttribute.h>
#    include <ImfStandardAttributes.h>
#    include <ImfStringAttribute.h>
#    include <ImfStringVectorAttribute.h>
#    include <ImfThreading.h>
#    include <ImfTileDescriptionAttribute.h>
#    include <ImfTimeCodeAttribute.h>
#    include <ImfVecAttribute.h>
#    pragma GCC diagnostic pop

#    include <algorithm>
#    include <iostream>
#endif

namespace media
{
#ifdef HAVE_OPENEXR
namespace EXR   = OPENEXR_IMF_NAMESPACE;
namespace IMATH = IMATH_NAMESPACE;

////////////////////////////////////////

namespace
{
class exr_ostream : public EXR::OStream
{
public:
    exr_ostream( base::ostream &str )
        : OStream( str.uri().c_str() ), _stream( str )
    {}
    virtual ~exr_ostream( void ) = default;

    virtual void write( const char c[], int n ) override
    {
        try
        {
            if ( !_stream.write( c, n ) )
                throw std::runtime_error( "Stream failed during write" );
        }
        catch ( ... )
        {
            std::throw_with_nested( std::runtime_error(
                "Error writing to OpenEXR stream: " + _stream.uri() ) );
        }
    }

    EXR::Int64 tellp( void ) override
    {
        try
        {
            return static_cast<EXR::Int64>( _stream.tellp() );
        }
        catch ( ... )
        {
            std::throw_with_nested( std::runtime_error(
                "Error telling position on OpenEXR output stream: " +
                _stream.uri() ) );
        }
    }

    void seekp( EXR::Int64 pos ) override
    {
        try
        {
            _stream.seekp( static_cast<std::streamoff>( pos ) );
        }
        catch ( ... )
        {
            std::throw_with_nested( std::runtime_error(
                "Error seeking to position on OpenEXR output stream: " +
                _stream.uri() ) );
        }
    }

private:
    base::ostream &_stream;
};

EXR::Compression extractCompression( const parameter_set &parms )
{
    EXR::Compression r = EXR::ZIP_COMPRESSION;

    auto c = parms.find( "compression" );
    if ( c != parms.end() && c->second.valid() )
    {
        const std::string &comp = c->second.as_string();
        if ( comp == "none" )
            r = EXR::NO_COMPRESSION;
        else if ( comp == "rle" )
            r = EXR::RLE_COMPRESSION;
        else if ( comp == "zips" )
            r = EXR::ZIPS_COMPRESSION;
        else if ( comp == "zip" )
            r = EXR::ZIP_COMPRESSION;
        else if ( comp == "piz" )
            r = EXR::PIZ_COMPRESSION;
        else if ( comp == "pxr24" )
            r = EXR::PXR24_COMPRESSION;
        else if ( comp == "b44" )
            r = EXR::B44_COMPRESSION;
        else if ( comp == "b44a" )
            r = EXR::RLE_COMPRESSION;
        else if ( comp == "dwaa" )
            r = EXR::DWAA_COMPRESSION;
        else if ( comp == "dwab" )
            r = EXR::DWAB_COMPRESSION;
        else
            throw_runtime(
                "Unknown compression scheme {0} given to OpenEXR", comp );
    }
    return r;
}

static inline int num_scanlines( EXR::Compression c )
{
    switch ( c )
    {
        case EXR::NO_COMPRESSION: return 1;
        case EXR::RLE_COMPRESSION: return -1;
        case EXR::ZIPS_COMPRESSION: return 1;
        case EXR::ZIP_COMPRESSION: return 16;

        case EXR::PIZ_COMPRESSION:
        case EXR::PXR24_COMPRESSION:
            // TODO: look at code to determine, for now pull whole image...
            return -1;

        case EXR::B44_COMPRESSION:
        case EXR::B44A_COMPRESSION: return 4;

        case EXR::DWAA_COMPRESSION: return 32;
        case EXR::DWAB_COMPRESSION: return 256;

        case EXR::NUM_COMPRESSION_METHODS:
        default: break;
    }
    return -1;
}

template <typename D, typename S>
static inline void do_copy( D &out, const S &in )
{
    static_assert(
        sizeof( D ) == sizeof( S ),
        "invalid size mismatch copying metadata values" );
    void *      outP = &out;
    const void *inP  = &in;
    memcpy( outP, inP, sizeof( D ) );
}

template <typename ET, typename T>
static inline void
set_meta( EXR::Header &header, const std::string &mname, const T &val )
{
    header.insert( mname, ET( val ) );
}

template <typename ET, typename T>
static inline void
set_meta_copy( EXR::Header &header, const std::string &mname, const T &val )
{
    header.insert( mname, ET() );
    do_copy( header.typedAttribute<ET>( mname ).value(), val );
}

static void test_meta_and_add(
    EXR::Header &header, const std::string &mname, const metadata_value &mval )
{
    switch ( mval.type() )
    {
        case meta_string_t::tag:
            if ( mname != "compression" )
                set_meta<EXR::StringAttribute>(
                    header, mname, meta_string_t::retrieve( mval ) );
            break;
        case meta_stringlist_t::tag:
            set_meta<EXR::StringVectorAttribute>(
                header, mname, meta_stringlist_t::retrieve( mval ) );
            break;
        case meta_int32_t::tag:
            if ( mname != "lineOrder" )
                set_meta<EXR::IntAttribute>(
                    header, mname, meta_int32_t::retrieve( mval ) );
            break;
        case meta_float32_t::tag:
            if ( mname != "whiteLuminance" )
                set_meta<EXR::FloatAttribute>(
                    header, mname, meta_float32_t::retrieve( mval ) );
            break;
        case meta_m33f_t::tag:
            set_meta_copy<EXR::M33fAttribute>(
                header, mname, meta_m33f_t::retrieve( mval ) );
            break;
        case meta_m44f_t::tag:
            set_meta_copy<EXR::M44fAttribute>(
                header, mname, meta_m44f_t::retrieve( mval ) );
            break;
        case meta_vec3f_t::tag:
            set_meta_copy<EXR::V3fAttribute>(
                header, mname, meta_vec3f_t::retrieve( mval ) );
            break;
        case meta_vec3i_t::tag:
            set_meta_copy<EXR::V3iAttribute>(
                header, mname, meta_vec3i_t::retrieve( mval ) );
            break;
        case meta_vec2f_t::tag:
            set_meta_copy<EXR::V2fAttribute>(
                header, mname, meta_vec2f_t::retrieve( mval ) );
            break;
        case meta_vec2i_t::tag:
            set_meta_copy<EXR::V2iAttribute>(
                header, mname, meta_vec2i_t::retrieve( mval ) );
            break;
        case meta_float64_t::tag:
            set_meta<EXR::DoubleAttribute>(
                header, mname, meta_float64_t::retrieve( mval ) );
            break;
        case meta_vec2d_t::tag:
            set_meta_copy<EXR::V2dAttribute>(
                header, mname, meta_vec2d_t::retrieve( mval ) );
            break;
        case meta_vec3d_t::tag:
            set_meta_copy<EXR::V3dAttribute>(
                header, mname, meta_vec3d_t::retrieve( mval ) );
            break;
        case meta_m33d_t::tag:
            set_meta_copy<EXR::M33dAttribute>(
                header, mname, meta_m33d_t::retrieve( mval ) );
            break;
        case meta_m44d_t::tag:
            set_meta_copy<EXR::M44dAttribute>(
                header, mname, meta_m44d_t::retrieve( mval ) );
            break;
        case meta_recti_t::tag:
            set_meta_copy<EXR::Box2iAttribute>(
                header, mname, meta_recti_t::retrieve( mval ) );
            break;
        case meta_rectf_t::tag:
            set_meta_copy<EXR::Box2fAttribute>(
                header, mname, meta_rectf_t::retrieve( mval ) );
            break;
        case meta_chromaticities_t::tag:
            if ( mname != "chromaticities" )
                set_meta_copy<EXR::ChromaticitiesAttribute>(
                    header, mname, meta_chromaticities_t::retrieve( mval ) );
            break;
        case meta_smpte_timecode_t::tag:
        {
            header.insert( mname, EXR::TimeCodeAttribute() );
            EXR::TimeCode &tc =
                header.typedAttribute<EXR::TimeCodeAttribute>( mname ).value();
            tc.setTimeAndFlags( meta_smpte_timecode_t::retrieve( mval ).first );
            tc.setUserData( meta_smpte_timecode_t::retrieve( mval ).second );
            break;
        }
        case meta_keycode_t::tag:
            set_meta_copy<EXR::KeyCodeAttribute>(
                header, mname, meta_keycode_t::retrieve( mval ) );
            break;
        case meta_rational32_t::tag:
            set_meta<EXR::RationalAttribute>(
                header,
                mname,
                EXR::Rational(
                    meta_rational32_t::retrieve( mval ).first,
                    static_cast<int>(
                        meta_rational32_t::retrieve( mval ).second ) ) );
            break;
        default:
            std::cerr << "WARNING: Unknown metadata value type " << mval.type()
                      << "('" << metadata_type_name( mval.type() )
                      << "') ignored writing EXR file" << std::endl;
            break;
    }
}

static void frame_to_headers(
    std::vector<EXR::Header> &headers,
    const frame &             frm,
    EXR::Compression          compression )
{
    // just to make sure it's set...
    tzset();

    for ( auto i = frm.image_begin(); i != frm.image_end(); ++i )
    {
        const image &img = ( *i );
        IMATH::Box2i disp, data;

        disp.min.x = img.full_area().x1();
        disp.min.y = img.full_area().y1();
        disp.max.x = img.full_area().x2();
        disp.max.y = img.full_area().y2();

        data.min.x = img.active_area().x1();
        data.min.y = img.active_area().y1();
        data.max.x = img.active_area().x2();
        data.max.y = img.active_area().y2();

        if ( !headers.empty() )
        {
            if ( disp != headers.back().displayWindow() )
                throw_runtime(
                    "All display windows must be the same for an EXR file" );
        }

        // TODO: handle screen center / width?
        // it will naturally fall through if someone is doing an exr to an exr
        // since we are copying metadata, but...
        // TODO: handle anything but increasing_y???
        headers.emplace_back(
            disp,
            data,
            img.aspect_ratio(),
            IMATH::V2f( 0, 0 ),
            1.f,
            EXR::INCREASING_Y,
            compression );

        EXR::Header &curheader = headers.back();
        // TODO: seems like this should be EXR_VERSION, but the code only
        // allows 1?
        curheader.setVersion( 1 /*EXR::EXR_VERSION*/ );

        EXR::ChannelList &chans = curheader.channels();
        for ( size_t p = 0; p < img.size(); ++p )
        {
            plane_layout pl = img.layout( p );

            if ( pl._bits == 16 && pl._floating )
                chans.insert( img.plane_name( p ), EXR::Channel( EXR::HALF ) );
            else if ( pl._bits == 32 && pl._floating )
                chans.insert( img.plane_name( p ), EXR::Channel( EXR::FLOAT ) );
            else if ( pl._bits == 32 )
                chans.insert( img.plane_name( p ), EXR::Channel( EXR::UINT ) );
            else
                throw_runtime( "Unsupported plane layout for OpenEXR" );
        }

        for ( auto &md: img.meta() )
            test_meta_and_add( curheader, md.first, md.second );

        if ( img.color_state().curve() != color::transfer::LINEAR )
            throw_runtime( "Attempt to save non-linear data to EXR file" );

        if ( img.color_state().scene_referred() )
            addWhiteLuminance(
                curheader,
                static_cast<float>( img.color_state().luminance_scale() ) );

        const auto &cx = img.color_state().chroma();
        set_meta<EXR::ChromaticitiesAttribute>(
            curheader,
            EXR::ChromaticitiesAttribute::staticTypeName(),
            EXR::Chromaticities(
                IMATH::V2f( cx.red.x, cx.red.y ),
                IMATH::V2f( cx.green.x, cx.green.y ),
                IMATH::V2f( cx.blue.x, cx.blue.y ),
                IMATH::V2f( cx.white.x, cx.white.y ) ) );

        std::string pname = i.access_layer().name();
        if ( pname.empty() )
        {
            if ( headers.size() > 1 )
                throw_runtime(
                    "Invalid empty name on layer with more than one layers" );
        }
        std::string vname = i.access_view().name();
        if ( vname.empty() )
        {
            if ( i.access_layer().view_count() > 1 )
                throw_runtime(
                    "Invalid empty name on view with more than one view in a layer" );
            curheader.setName( pname );
        }
        else
        {
            curheader.setView( vname );
            curheader.setName( pname + "." + vname );
        }
        // TODO: handle tiled image
        curheader.setType( EXR::SCANLINEIMAGE );

        auto stime = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() );
        struct tm ltime;
        // TODO: locale?
        std::stringstream sbuf;
        // hrm, comment in ImfStandardAttributes has format, and it's not exactly ISO 8601
        struct tm *ltimep = localtime_r( &stime, &ltime );
        sbuf << std::put_time( ltimep, "%Y:%m:%d %H:%M:%S" );
        addCapDate( curheader, sbuf.str() );
        addUtcOffset(
            curheader, static_cast<float>( base::local_offset_to_UTC() ) );
    }

    if ( frm.data_begin() != frm.data_end() )
        throw_logic( "Writing of deep data not yet finished" );
}

static void rebuild_frame_buffer(
    EXR::FrameBuffer &               fB,
    const image &                    img,
    const std::vector<image_buffer> &bufs )
{
    precondition(
        bufs.size() == img.size(), "expecting calc_bufs to fill buffers" );

    const area_rect &dataWin = img.active_area();
    int64_t          w       = dataWin.width();
    int64_t          baseOff = dataWin.x1() + dataWin.y1() * w;
    for ( size_t p = 0; p < img.size(); ++p )
    {
        const image_buffer &ib         = bufs[p];
        int64_t             curScanOff = ( ib.y1() - dataWin.y1() ) * w;
        size_t offTotal = static_cast<size_t>( baseOff + curScanOff );
        if ( ib.bits() == 16 && ib.is_floating() )
        {
            char *data =
                ( const_cast<char *>( static_cast<const char *>( ib.data() ) ) -
                  offTotal * sizeof( base::half ) );
            fB.insert(
                img.plane_name( p ),
                EXR::Slice(
                    EXR::HALF,
                    data,
                    static_cast<size_t>( ib.xstride_bytes() ),
                    static_cast<size_t>( ib.ystride_bytes() ) ) );
        }
        else if ( ib.bits() == 32 && ib.is_floating() )
        {
            char *data =
                ( const_cast<char *>( static_cast<const char *>( ib.data() ) ) -
                  offTotal * sizeof( float ) );
            fB.insert(
                img.plane_name( p ),
                EXR::Slice(
                    EXR::FLOAT,
                    data,
                    static_cast<size_t>( ib.xstride_bytes() ),
                    static_cast<size_t>( ib.ystride_bytes() ) ) );
        }
        else if ( ib.bits() == 32 )
        {
            char *data =
                ( const_cast<char *>( static_cast<const char *>( ib.data() ) ) -
                  offTotal * sizeof( uint32_t ) );
            fB.insert(
                img.plane_name( p ),
                EXR::Slice(
                    EXR::UINT,
                    data,
                    static_cast<size_t>( ib.xstride_bytes() ),
                    static_cast<size_t>( ib.ystride_bytes() ) ) );
        }
        else
            throw_runtime( "Invalid image buffer for writing to EXR" );
    }
}

class exr_write_track final : public video_track
{
public:
    exr_write_track( const base::uri &files, const parameter_set &parms )
        : video_track(
              std::string(),
              std::string(),
              0,
              0,
              sample_rate(),
              track_description( TRACK_VIDEO ) )
        , _files( files )
        , _compression( extractCompression( parms ) )
    {}

    exr_write_track(
        std::string                     n,
        std::string                     v,
        int64_t                         b,
        int64_t                         e,
        const sample_rate &             sr,
        const base::uri &               files,
        const media::track_description &td,
        const parameter_set &           parms )
        : video_track( std::move( n ), std::move( v ), b, e, sr, td )
        , _files( files )
        , _compression( extractCompression( parms ) )
    {}

    ~exr_write_track( void ) override {}

    frame *doRead( int64_t ) override
    {
        throw_logic( "writer asked to read a frame" );
    }

    void doWrite( int64_t f, const frame &frm, base::allocator &a ) override
    {
        auto          fs     = base::file_system::get( _files.uri() );
        base::ostream stream = fs->open_write( _files.get_frame( f ) );
        exr_ostream   estr( stream );

        std::vector<EXR::Header> headers;
        frame_to_headers( headers, frm, _compression );

        EXR::MultiPartOutputFile outfile{ estr,
                                          headers.data(),
                                          static_cast<int>( headers.size() ) };

        int partNum = 0;
        // TODO: handle tiles
        int numScans = num_scanlines( _compression );

        // TODO: does this need to move up a level?
        std::vector<image_buffer> bufs;
        for ( auto i = frm.image_begin(); i != frm.image_end(); ++i, ++partNum )
        {
            EXR::OutputPart curOut( outfile, partNum );

            image &img       = ( *i );
            auto   prefchunk = img.preferred_chunk_size();

            int pullScans = std::max( numScans, prefchunk.second );
            int y         = img.active_area().y1();
            int yend      = img.active_area().y2() + 1;

            // TODO: handle tiles
            precondition(
                prefchunk.first == img.active_area().width(),
                "Expect preferred chunk X size to be width of image" );
            img.create_buffers( bufs, a, true );

            area_rect curarea = img.active_area();
            // TODO: support anything but increasing y?
            while ( y < yend )
            {
                int ey = y + pullScans;
                if ( ey > yend )
                    ey = yend;
                int curScans = ey - y;

                // TODO: handle tiles
                for ( auto &p: bufs )
                {
                    p.reset_position( curarea.x1(), y );
                    // at the end, we might make it shorter, but that's ok
                    p.force_height( curScans );
                }

                img.extract_image( bufs );

                // hrm, this seems inefficient, but exr also copies all this stuff all the time
                EXR::FrameBuffer fbuf;
                rebuild_frame_buffer( fbuf, img, bufs );

                curOut.setFrameBuffer( fbuf );
                curOut.writePixels( curScans );

                y = ey;
            }
        }

        if ( frm.data_begin() != frm.data_end() )
            throw_logic( "Writing of deep data not yet finished" );
    }

private:
    file_sequence    _files;
    EXR::Compression _compression;
};

////////////////////////////////////////

class OpenEXRWriter : public writer
{
public:
    OpenEXRWriter( void ) : writer( "OpenEXR" )
    {
        _description = "OpenEXR Writer";
        _extensions.emplace_back( "exr" );
        _extensions.emplace_back( "sxr" );
        _extensions.emplace_back( "mxr" ); // multi-view EXR
        _extensions.emplace_back( "aces" );
        _parms.push_back( media::parameter_definition(
            "compression",
            std::vector<std::string>{ "none",
                                      "rle",
                                      "zips",
                                      "zip",
                                      "piz"
                                      "pxr24",
                                      "b44",
                                      "b44a",
                                      "dwaa",
                                      "dwab" },
            "zip" ) );
        _parms.back().help(
            "Set the compression style:\n"
            " none   - No compression\n"
            " zips   - Zip, one scanline per zip chunk\n"
            " zip    - Zip, 16 scanlines per chunk\n"
            " piz    - Piz-based wavelet compression\n"
            " pxr24  - Lossy 24-bit float compression\n"
            " b44    - Lossy 4-by-4 compression\n"
            " b44a   - Lossy 4-by-4, flat fields are compressed more\n"
            " dwaa   - Lossy DCT in blocks of 32 scanlines\n"
            " dwab   - Lossy DCT in blocks of 256 scanlines\n" );
    }
    virtual ~OpenEXRWriter( void ) = default;

    virtual container create(
        const base::uri &                     u,
        const std::vector<track_description> &td,
        const parameter_set &                 params );
};

container OpenEXRWriter::create(
    const base::uri &                     u,
    const std::vector<track_description> &tdlist,
    const parameter_set &                 params )
{
    container ret;

    if ( tdlist.empty() )
    {
        ret.add_track( std::make_shared<exr_write_track>( u, params ) );
    }
    else
    {
        const track_description &td = tdlist.front();
        if ( tdlist.size() != 1 || td.type() != TRACK_VIDEO )
            throw_runtime(
                "OpenEXR only supports a single video track right now" );

        ret.add_track( std::make_shared<exr_write_track>(
            td.name(),
            td.view(),
            td.offset(),
            td.offset() + td.duration() - 1,
            td.rate(),
            u,
            td,
            params ) );
    }
    ret.set_parameters( params );

    return ret;
}

} // namespace
#endif

void register_exr_writer( void )
{
#if defined( HAVE_OPENEXR )
    if ( EXR::globalThreadCount() == 0 )
        EXR::setGlobalThreadCount(
            static_cast<int>( base::thread::core_count() ) );

    writer::register_writer( std::make_shared<OpenEXRWriter>() );
#endif
}

////////////////////////////////////////

} // namespace media
