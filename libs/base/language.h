//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <locale>

////////////////////////////////////////

namespace base
{

namespace locale
{

void set( const std::string &l );
std::string language( void );
// if available, returns the locale for language specified
// (mostly a cross-platform function)
std::locale query( const std::string &l );

}

}



