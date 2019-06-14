// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include "check.h"

#include "opengl.h"

#include <iostream>
#include <sstream>

namespace gl
{
////////////////////////////////////////

void error_check( size_t line )
{
    GLenum err = glGetError();
    while ( err != GL_NO_ERROR )
    {
        std::stringstream str;
        str << "opengl error " << err << ": ";
        switch ( err )
        {
            case GL_INVALID_ENUM: str << "invalid enum "; break;
            case GL_INVALID_VALUE: str << "invalid value "; break;
            case GL_INVALID_OPERATION: str << "invalid operation "; break;
            case GL_OUT_OF_MEMORY: str << "out of memory "; break;
            default: str << "unknown "; break;
        }
        str << "at line " << line;
        std::cerr << str.str() << std::endl;
        //		throw std::runtime_error( str.str() );

        err = glGetError();
    }
}

////////////////////////////////////////

} // namespace gl
