// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "compiler_support.h"

#include <ostream>
#include <string>
#include <typeinfo>

////////////////////////////////////////

namespace base
{
std::string        demangle( const char *sym );
inline std::string demangle( const std::string &sym )
{
    return demangle( sym.c_str() );
}
inline std::string demangle( const std::type_info &t )
{
    return demangle( t.name() );
}

} // namespace base
