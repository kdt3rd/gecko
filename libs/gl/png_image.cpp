// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "png_image.h"

#include <base/contract.h>
#include <base/scope_guard.h>
#include <png.h>
#include <vector>

namespace gl
{
////////////////////////////////////////

namespace
{
std::vector<uint8_t>
png_read_data( const char *file_name, size_t &w, size_t &h, size_t &c )
{
    FILE *fp = fopen( file_name, "rb" );
    if ( fp == nullptr )
        throw_errno( "error opening file {0}", file_name );
    on_scope_exit { fclose( fp ); };

    // read the header
    png_byte header[8];
    fread( header, 1, 8, fp );

    if ( png_sig_cmp( header, 0, 8 ) )
        throw_runtime( "error: {0} is not a PNG.\n", file_name );

    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
    if ( !png_ptr )
        throw_runtime( "error: png_create_read_struct returned 0" );
    on_scope_exit { png_destroy_read_struct( &png_ptr, nullptr, nullptr ); };

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
    png_init_io( png_ptr, fp );

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

    if ( bit_depth > 8 )
        png_set_strip_16( png_ptr );

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
    if ( bit_depth != 8 )
        throw_runtime( "only 8 bit PNG images supported" );

    switch ( color_type )
    {
        case PNG_COLOR_TYPE_GRAY: c = 1; break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: c = 2; break;
        case PNG_COLOR_TYPE_RGB: c = 3; break;
        case PNG_COLOR_TYPE_RGB_ALPHA: c = 4; break;
        default:
            throw_runtime( "PNG color type not supported ({0})", color_type );
    }

    // Row size in bytes.
    png_size_t rowbytes = png_get_rowbytes( png_ptr, info_ptr );

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ( ( rowbytes - 1 ) % 4 );

    // Allocate the image_data as a big block, to be given to opengl
    std::vector<uint8_t> image_data;
    image_data.resize( rowbytes * temp_height + 15 );

    // row_pointers is for pointing to image_data for reading the png with libpng
    std::vector<png_bytep> row_pointers;
    row_pointers.resize( temp_height );

    // set the individual row_pointers to point at the correct offsets of image_data
    for ( png_uint_32 i = 0; i < temp_height; i++ )
        row_pointers[i] = image_data.data() + i * rowbytes;

    // read the png into image_data through row_pointers
    png_read_image( png_ptr, row_pointers.data() );

    w = static_cast<size_t>( temp_width );
    h = static_cast<size_t>( temp_height );

    return image_data;
}

} // namespace

////////////////////////////////////////

void png_write( const char *file_name, size_t w, size_t h, size_t c )
{
    precondition( w > 0, "width must be larger than 0" );
    precondition( h > 0, "height must be larger than 0" );
    GLenum fmt;
    switch ( c )
    {
        case 1: fmt = GL_RED; break;
        case 3: fmt = GL_RGB; break;
        case 4: fmt = GL_RGBA; break;
        default: throw_runtime( "invalid number of channels ({0})", c );
    }

    std::unique_ptr<uint8_t[]> img( new uint8_t[w * h * c] );
    GLint                      align = 0;
    glGetIntegerv( GL_PACK_ALIGNMENT, &align );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadPixels(
        0,
        0,
        static_cast<GLsizei>( w ),
        static_cast<GLsizei>( h ),
        fmt,
        GL_UNSIGNED_BYTE,
        img.get() );
    glPixelStorei( GL_PACK_ALIGNMENT, align );

    png_write_data( file_name, w, h, c, img.get(), true );
}

////////////////////////////////////////

void png_write_data(
    const char *   file_name,
    size_t         w,
    size_t         h,
    size_t         c,
    const uint8_t *img,
    bool           flip )
{
    int png_ctype;
    switch ( c )
    {
        case 1: png_ctype = PNG_COLOR_TYPE_GRAY; break;
        case 3: png_ctype = PNG_COLOR_TYPE_RGB; break;
        case 4: png_ctype = PNG_COLOR_TYPE_RGB_ALPHA; break;
        default: throw_runtime( "invalid number of channels ({0})", c );
    }

    FILE *fp = fopen( file_name, "wb" );
    if ( fp == nullptr )
        throw_errno( "error opening file {0}", file_name );
    on_scope_exit { fclose( fp ); };

    // Initialize write structure
    auto png_ptr =
        png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
    if ( png_ptr == nullptr )
        throw_runtime( "could not allocate write structure" );
    on_scope_exit { png_destroy_write_struct( &png_ptr, nullptr ); };

    // Initialize info structure
    auto info_ptr = png_create_info_struct( png_ptr );
    if ( info_ptr == nullptr )
        throw_runtime( "could not allocate info structure" );
    on_scope_exit { png_free_data( png_ptr, info_ptr, PNG_FREE_ALL, -1 ); };

    // Setup Exception handling
    if ( setjmp( png_jmpbuf( png_ptr ) ) != 0 )
        throw_runtime( "error during PNG creation" );

    png_init_io( png_ptr, fp );

    // Write header (8 bit color depth)
    png_set_IHDR(
        png_ptr,
        info_ptr,
        static_cast<png_uint_32>( w ),
        static_cast<png_uint_32>( h ),
        8,
        png_ctype,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE );
    png_write_info( png_ptr, info_ptr );

    // Write image data
    if ( flip )
    {
        for ( size_t y = 0; y < h; ++y )
            png_write_row( png_ptr, img + ( h - y - 1 ) * w * c );
    }
    else
    {
        for ( size_t y = 0; y < h; ++y )
            png_write_row( png_ptr, img + y * w * c );
    }

    // End write
    png_write_end( png_ptr, nullptr );
    std::cout << "Saved png image to '" << file_name << "'" << std::endl;
}

////////////////////////////////////////

gl::texture png_read( const char *file_name )
{
    size_t               w, h, c;
    std::vector<uint8_t> data = png_read_data( file_name, w, h, c );
    GLenum               fmt;
    switch ( c )
    {
        case 1: fmt = GL_RED; break;
        case 2: fmt = GL_RG; break;
        case 3: fmt = GL_RGB; break;
        case 4: fmt = GL_RGBA; break;
        default: throw_runtime( "invalid channels from PNG" );
    }

    // Generate the OpenGL texture object
    GLuint txt = 0;
    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &txt );
    glBindTexture( GL_TEXTURE_RECTANGLE, txt );
    glTexImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        GL_RGBA,
        static_cast<GLsizei>( w ),
        static_cast<GLsizei>( h ),
        0,
        fmt,
        GL_UNSIGNED_BYTE,
        data.data() );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    return gl::texture( txt, gl::texture::target::RECTANGLE );
}

////////////////////////////////////////

} // namespace gl
