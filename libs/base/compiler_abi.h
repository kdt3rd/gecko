// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <typeinfo>
#include <ostream>

#include "compiler_support.h"

////////////////////////////////////////

namespace base
{

std::string demangle( const char *sym );
inline std::string demangle( const std::string &sym )
{
	return demangle( sym.c_str() );
}
inline std::string demangle( const std::type_info &t )
{
	return demangle( t.name() );
}

} // namespace base
