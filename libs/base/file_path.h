
#pragma once

#include <string>

namespace base
{

////////////////////////////////////////

std::string remove_extension( const std::string &file );
std::string remove_extension( std::string &&file );

////////////////////////////////////////

std::string basename( const std::string &file );

////////////////////////////////////////

}

