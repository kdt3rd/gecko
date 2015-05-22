
#pragma once

#include <string>

namespace base
{

////////////////////////////////////////

size_t base64_length( size_t bytes );

std::string base64_encode( const void *data, size_t bytes );
std::string base64_decode( const void *data, size_t bytes );

inline std::string base64_encode( const std::string &data )
{
	return base64_encode( data.c_str(), data.size() );
}

inline std::string base64_decode( const std::string &data )
{
	return base64_decode( data.c_str(), data.size() );
}

////////////////////////////////////////

}

