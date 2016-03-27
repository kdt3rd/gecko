
#pragma once

#include <string>

namespace base
{

////////////////////////////////////////

/// if the file name is blah.exr, the extension
/// returned will be 'exr'
std::string get_extension( const std::string &file );

std::string remove_extension( const std::string &file );
std::string remove_extension( std::string &&file );

////////////////////////////////////////

std::string basename( const std::string &file );

////////////////////////////////////////

}

