//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <windows.h>
#include <gl/opengl.h>
#include "system.h"

////////////////////////////////////////

namespace platform
{
namespace mswin
{

platform::system::opengl_func_ptr queryGL( const char *f );
HGLRC createOGLContext( HDC dc );

} // namespace mswin
} // namespace platform



