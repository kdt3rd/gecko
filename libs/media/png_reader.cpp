// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "png_reader.h"

#if defined( HAVE_LIBPNG )
#    include "file_per_sample_reader.h"
#    include "file_sequence.h"
#    include "image_frame.h"
#    include "video_track.h"

#    include <base/contract.h>
#    include <base/env.h>
#    include <base/file_system.h>
#    include <base/scope_guard.h>
#    include <base/string_util.h>
#    include <png.h>
#    ifndef _WIN32
#        include <setjmp.h>
#    endif

#endif // HAVE_LIBPNG

#include <media/riff/fourcc.h>

namespace
{
// tbd: move this to central place for reader and writer
struct chunk
{
    // always have len, type and crc. crc includes type and (data)
    uint32_t             _len;
    media::fourcc        _type;
    uint32_t             _crc;
    std::vector<uint8_t> data; // TBD: zero copy, subimage read
};
} // namespace
////////////////////////////////////////

namespace media
{
#if defined( HAVE_LIBPNG )
namespace
{
struct StreamHolder
{
    base::istream *stream;
#    ifndef _WIN32
    sigjmp_buf jmpBuffer;
#    endif
    ssize_t nread;
    ssize_t nreq;
};

void mypng_raw_read( png_structp png_ptr, png_bytep data, png_size_t length )
{
    StreamHolder *readPtr =
        static_cast<StreamHolder *>( png_get_io_ptr( png_ptr ) );
    readPtr->stream->read( (char *)data, length );
    readPtr->nread = readPtr->stream->gcount();
    if ( readPtr->nread != ssize_t( length ) )
    {
        readPtr->nreq = ssize_t( length );
#    ifndef _WIN32
        longjmp( readPtr->jmpBuffer, 1 );
#    endif
    }
}

class png_read_track : public video_track
{
public:
    png_read_track( int64_t b, int64_t e, file_sequence &&fseq )
        : video_track(
              "<image>",
              std::string(),
              b,
              e,
              sample_rate( 24, 1 ),
              media::track_description( media::TRACK_VIDEO ) )
        , _files( std::move( fseq ) )
    {}
    ~png_read_track( void ) override = default;

    image_frame *doRead( int64_t f ) override;

    void doWrite( int64_t, const image_frame & )
    {
        throw_logic( "reader asked to write a frame" );
    }

    void doWrite( int64_t, const std::vector<std::shared_ptr<image_frame>> & )
    {
        throw_logic( "reader asked to write a frame" );
    }

private:
    file_sequence _files;
};

image_frame *png_read_track::doRead( int64_t f )
{
    std::unique_ptr<image_frame> ret;

    base::uri     u      = _files.get_frame( f );
    std::string   p      = u.full_path();
    auto          fs     = base::file_system::get( u );
    base::istream stream = fs->open_read( u );
    StreamHolder  sh;
    sh.stream = &stream;

    png_byte header[8];
    stream.read( (char *)header, 8 );

    if ( png_sig_cmp( header, 0, 8 ) )
        throw_runtime( "error: {0} is not a PNG.\n", p );

    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
    if ( !png_ptr )
        throw_runtime( "error: png_create_read_struct returned 0" );
    on_scope_exit { png_destroy_read_struct( &png_ptr, nullptr, nullptr ); };

#    ifndef _WIN32
    if ( setjmp( sh.jmpBuffer ) )
        throw_runtime(
            "error: unable to read requested bytes {0} from stream, got {1}",
            sh.nreq,
            sh.nread );
#    endif

    // create png info struct
    png_infop info_ptr = png_create_info_struct( png_ptr );
    if ( !info_ptr )
        throw_runtime( "error: png_create_info_struct returned 0" );

    // create png info struct
    png_infop end_info = png_create_info_struct( png_ptr );
    if ( !end_info )
        throw_runtime( "error: png_create_info_struct returned 0" );

    // the code in this if statement gets called if libpng encounters an error
    if ( setjmp( png_jmpbuf( png_ptr ) ) )
        throw_runtime( "error from libpng" );

    // init png reading
    png_set_read_fn( png_ptr, &sh, mypng_raw_read );
    //  png_init_io( png_ptr, fp );

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes( png_ptr, 8 );

    // read all the info up to the image data
    png_read_info( png_ptr, info_ptr );

    // variables to pass to get info
    int         bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    if ( png_get_IHDR(
             png_ptr,
             info_ptr,
             &temp_width,
             &temp_height,
             &bit_depth,
             &color_type,
             nullptr,
             nullptr,
             nullptr ) == 0 )
        throw_runtime( "error getting PNG header" );

    double fgama = 1.0;
    if ( png_get_gAMA( png_ptr, info_ptr, &fgama ) == 0 )
        fgama = 2.4;

    if ( color_type == PNG_COLOR_TYPE_PALETTE )
    {
        png_set_palette_to_rgb( png_ptr );
        png_set_tRNS_to_alpha( png_ptr );
    }

    if ( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
        png_set_expand_gray_1_2_4_to_8( png_ptr );

    if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        png_set_tRNS_to_alpha( png_ptr );

    // Update the png info struct.
    png_read_update_info( png_ptr, info_ptr );
    if ( png_get_IHDR(
             png_ptr,
             info_ptr,
             &temp_width,
             &temp_height,
             &bit_depth,
             &color_type,
             nullptr,
             nullptr,
             nullptr ) == 0 )
        throw_runtime( "error getting PNG header" );

    // Check the info to make sure we can use it.
    //  if ( bit_depth != 8 )
    //      throw_runtime( "only 8 bit PNG images supported" );

    ret.reset( new image_frame( 0, 0, temp_width - 1, temp_height - 1 ) );

    int c = 0;
    switch ( color_type )
    {
        case PNG_COLOR_TYPE_GRAY: c = 1; break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: c = 2; break;
        case PNG_COLOR_TYPE_RGB: c = 3; break;
        case PNG_COLOR_TYPE_RGB_ALPHA: c = 4; break;
        default:
            throw_runtime( "PNG color type not supported ({0})", color_type );
    }

    image_buffer imgbuf;
    int          offbits = 8;
    if ( bit_depth > 8 )
    {
        offbits = 16;
        // png is always in network (BIG endian) order...
        imgbuf = image_buffer::simple_interleaved<uint16_t>(
            temp_width, temp_height, c, base::endianness::BIG );
    }
    else
        imgbuf = image_buffer::simple_interleaved<uint8_t>(
            temp_width, temp_height, c );

    const char *chanNames[] = { "R", "G", "B", "A" };
    if ( color_type == PNG_COLOR_TYPE_GRAY )
    {
        ret->add_channel( "R", image_buffer( imgbuf, offbits * 0 ) );
        ret->add_channel( "A", image_buffer( imgbuf, offbits * 1 ) );
    }
    else
    {
        precondition(
            size_t( c ) <= ( sizeof( chanNames ) / sizeof( const char * ) ),
            "valid channel count" );
        for ( int x = 0; x < c; ++x )
            ret->add_channel(
                chanNames[x], image_buffer( imgbuf, offbits * x ) );
    }

    // Row size in bytes.
    png_size_t rowbytes = png_get_rowbytes( png_ptr, info_ptr );

    std::vector<png_bytep> row_pointers;
    row_pointers.resize( temp_height, nullptr );

    if ( png_size_t( imgbuf.ystride_bytes() ) < rowbytes )
        throw_runtime(
            "PNG unexpected number of bytes for row({0}), expect {1}",
            rowbytes,
            imgbuf.ystride_bytes() );

    for ( png_uint_32 i = 0; i < temp_height; ++i )
        row_pointers[i] =
            static_cast<uint8_t *>( imgbuf.row( static_cast<int64_t>( i ) ) );

    // read the png into image_data through row_pointers
    png_read_image( png_ptr, row_pointers.data() );

    return ret.release();
}

class PNG_reader : public file_per_sample_reader
{
public:
    PNG_reader( void ) : file_per_sample_reader( "PNG" )
    {
        _description = "PNG Reader";
        _extensions.emplace_back( "png" );
        std::vector<uint8_t> magic{ 0x89, 0x50, 0x4e, 0x47,
                                    0x0d, 0x0a, 0x1a, 0x0a };
        _magics.emplace_back( std::move( magic ) );
    }
    ~PNG_reader( void ) override = default;

    container create( const base::uri &u, const metadata &params ) override;
};

container PNG_reader::create( const base::uri &u, const metadata &params )
{
    container result;

    int64_t       start, last;
    file_sequence fseq( u );
    auto          fs = scan_samples( start, last, fseq );

    result.add_track(
        std::make_shared<png_read_track>( start, last, std::move( fseq ) ) );
    return result;
}

} // namespace
#endif // HAVE_LIBPNG

////////////////////////////////////////

void register_png_reader( void )
{
#if defined( HAVE_LIBPNG )
    reader::register_reader( std::make_shared<PNG_reader>() );
#endif
}

} // namespace media
