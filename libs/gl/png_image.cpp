
#include "png_image.h"

#include <vector>
#include <png.h>
#include <base/contract.h>
#include <base/scope_guard.h>


namespace gl
{

////////////////////////////////////////

void png_write( const char *file_name, size_t w, size_t h, size_t c )
{
	precondition( w > 0, "width must be larger than 0" );
	precondition( h > 0, "height must be larger than 0" );
	GLenum fmt;
	int png_ctype;
	switch ( c )
	{
		case 1: fmt = GL_RED; png_ctype = PNG_COLOR_TYPE_GRAY; break;
		case 3: fmt = GL_RGB; png_ctype = PNG_COLOR_TYPE_RGB; break;
		case 4: fmt = GL_RGBA; png_ctype = PNG_COLOR_TYPE_RGB_ALPHA; break;
		default: throw_runtime( "invalid number of channels ({0})", c );
	}

	std::unique_ptr<uint8_t[]> img( new uint8_t[w*h*c] );
	glReadPixels( 0, 0, static_cast<GLsizei>( w ), static_cast<GLsizei>( h ), fmt, GL_UNSIGNED_BYTE, img.get() );

	FILE *fp = fopen( file_name, "wb" );
	if ( fp == nullptr )
		throw_errno( "error opening file {0}", file_name );
	on_scope_exit
	{
		fclose( fp );
	};

	// Initialize write structure
	auto png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( png_ptr == NULL )
		throw_runtime( "could not allocate write structure" );
	on_scope_exit
	{
		png_destroy_write_struct( &png_ptr, nullptr );
	};

	// Initialize info structure
	auto info_ptr = png_create_info_struct( png_ptr );
	if (info_ptr == NULL)
		throw_runtime( "could not allocate info structure" );
	on_scope_exit
	{
		png_free_data( png_ptr, info_ptr, PNG_FREE_ALL, -1 );
	};

	// Setup Exception handling
	if ( setjmp( png_jmpbuf( png_ptr ) ) != 0 )
		throw_runtime( "error during PNG creation" );

	png_init_io( png_ptr, fp );

	// Write header (8 bit colour depth)
	png_set_IHDR( png_ptr, info_ptr, static_cast<png_uint_32>( w ), static_cast<png_uint_32>( h ), 8, png_ctype, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );
	png_write_info( png_ptr, info_ptr );

	// Write image data
	for ( size_t y = 0; y < h; ++y )
		png_write_row( png_ptr, img.get() + ( h - y - 1 ) * w * c );

	// End write
	png_write_end( png_ptr, NULL );
}

////////////////////////////////////////

gl::texture png_read( const char *file_name )
{
	FILE *fp = fopen( file_name, "rb" );
	if ( fp == nullptr )
		throw_errno( "error opening file {0}", file_name );
	on_scope_exit
	{
		fclose( fp );
	};

	// read the header
	png_byte header[8];
	fread( header, 1, 8, fp );

	if (png_sig_cmp(header, 0, 8))
		throw_runtime( "error: {0} is not a PNG.\n", file_name );

	png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
	if (!png_ptr)
		throw_runtime( "error: png_create_read_struct returned 0" );
	on_scope_exit
	{
		png_destroy_read_struct( &png_ptr, nullptr, nullptr );
	};

	// create png info struct
	png_infop info_ptr = png_create_info_struct( png_ptr );
	if ( !info_ptr )
		throw_runtime( "error: png_create_info_struct returned 0" );

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
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
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR( png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes-1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	std::vector<png_byte> image_data;
	image_data.resize( rowbytes * temp_height + 15 );

	// row_pointers is for pointing to image_data for reading the png with libpng
	std::vector<png_bytep> row_pointers;
	row_pointers.resize( temp_height );

	// set the individual row_pointers to point at the correct offsets of image_data
	for ( png_uint_32 i = 0; i < temp_height; i++)
		row_pointers[i] = image_data.data() + i * rowbytes;

	// read the png into image_data through row_pointers
	png_read_image( png_ptr, row_pointers.data() );

	// Generate the OpenGL texture object
	GLuint txt = 0;
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &txt );
	glBindTexture( GL_TEXTURE_RECTANGLE, txt );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA, static_cast<GLsizei>( temp_width ), static_cast<GLsizei>( temp_height ), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data() );
	glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	return gl::texture( txt );
}

////////////////////////////////////////

}

