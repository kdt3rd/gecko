
#pragma once

#ifdef __APPLE__
#	include <OpenGL/gl3.h>
#endif

#ifdef _WIN32
#define GLEW_STATIC
#	include <glew.h>
#	include <wglew.h>
#endif

#ifdef __unix__
#	include <GL/glxew.h>
#endif

