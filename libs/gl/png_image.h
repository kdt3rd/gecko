//
// Copyright (c) 2013-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "texture.h"

namespace gl
{

////////////////////////////////////////

/// @brief Write the current framebuffer to a PNG file.
void png_write( const char *file_name, size_t w, size_t h, size_t c );

void png_write_data( const char *file_name, size_t w, size_t h, size_t c, const uint8_t *buf, bool flip );

/// @brief Load the PNG file into a texture.
gl::texture png_read( const char *file_name );

////////////////////////////////////////

}

