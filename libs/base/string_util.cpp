
#include "string_util.h"

namespace core
{

////////////////////////////////////////

std::string trim( const std::string &str, const std::string &ws )
{
    auto begin = str.find_first_not_of( ws );
    if ( begin == std::string::npos )
        return std::string();

    auto end = str.find_last_not_of( ws );
    return str.substr( begin, end - begin + 1 );
}

////////////////////////////////////////

}

