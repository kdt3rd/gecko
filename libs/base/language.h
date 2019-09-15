// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <locale>
#include <string>

////////////////////////////////////////

namespace base
{
namespace locale
{
void        set( const std::string &l );
std::string language( void );
// if available, returns the locale for language specified
// (mostly a cross-platform function)
std::locale query( const std::string &l );

} // namespace locale

} // namespace base
