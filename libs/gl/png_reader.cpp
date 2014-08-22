
#include "png_reader.h"

#include <vector>
#include <png.h>
#include <base/contract.h>
#include <base/scope_guard.h>


namespace gl
{

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
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

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
	GLuint txt;
	glGenTextures( 1, &txt );
	glBindTexture( GL_TEXTURE_RECTANGLE, txt );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA, temp_width, temp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data() );
	glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	return gl::texture( txt );
}

////////////////////////////////////////

}

