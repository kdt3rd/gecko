// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <string>

namespace base
{
////////////////////////////////////////

/// if the file name is blah.exr, the extension
/// returned will be 'exr'
std::string file_extension( const std::string &file );

std::string remove_extension( const std::string &file );
std::string remove_extension( std::string &&file );

////////////////////////////////////////

std::string basename( const std::string &file );
std::string dirname( const std::string &file );

////////////////////////////////////////

} // namespace base
