//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#ifdef __APPLE__
#	include <OpenGL/gl3.h>
#endif

#ifdef _WIN32
#	include "gl3w.h"
#	include <GL/wglext.h>
#endif

#ifdef __unix__
#	include "gl3w.h"
#	include <GL/glx.h>
#endif

