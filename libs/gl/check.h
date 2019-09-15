// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstdlib>

#define checkgl()                                                              \
    do                                                                         \
    {                                                                          \
        gl::error_check( __LINE__ );                                           \
    } while ( false )

namespace gl
{
void error_check( size_t line );
}
