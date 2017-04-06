//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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

std::string rtrim( const std::string &str, const std::string &ws )
{
    auto end = str.find_last_not_of( ws );
	if ( end == std::string::npos )
		return std::string();

    return str.substr( 0, end + 1 );
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

std::string replace( std::string &&str, char c, const cstring &replacement )
{
	std::string result( std::move( str ) );
	size_t pos = 0;
	pos = result.find( c, pos );
	while ( pos != std::string::npos )
	{
		result.replace( pos, 1, replacement.c_str(), replacement.size() );
		pos = result.find( c, pos + replacement.size() );
	}
	return result;
}

////////////////////////////////////////

}

