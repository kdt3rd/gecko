//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
