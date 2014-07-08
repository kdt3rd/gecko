
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

