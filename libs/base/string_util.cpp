
#include "string_util.h"

namespace base
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

std::string ltrim( const std::string &str, const std::string &ws )
{
    auto begin = str.find_first_not_of( ws );
    if ( begin == std::string::npos )
        return std::string();

    return str.substr( begin );
}

////////////////////////////////////////

std::string rtrim( std::string str, const std::string &ws )
{
	while ( !str.empty() && ws.find( str.back() ) != std::string::npos )
		str.pop_back();
	return std::move( str );
}

////////////////////////////////////////

std::string replace( std::string &&str, char c, const std::string &replacement )
{
	std::string result( std::move( str ) );
	size_t pos = 0;
	pos = result.find( c, pos );
	while ( pos != std::string::npos )
	{
		result.replace( pos, 1, replacement );
		pos = result.find( c, pos + replacement.size() );
	}
	return result;
}

////////////////////////////////////////

}

