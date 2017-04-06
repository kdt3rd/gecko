//
// Copyright (c) 2015-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>

namespace base
{

////////////////////////////////////////

/// @brief Calculate how many bytes the Base64 encoding will be.
/// @param bytes Number of data bytes
/// @returns Number of base64 bytes.
size_t base64_length( size_t bytes );

/// @brief Encode the data into a base64 string.
std::string base64_encode( const void *data, size_t bytes );

/// @brief Decode the data from a base64 string.
std::string base64_decode( const void *data, size_t bytes );

/// @brief Encode the data into a base64 string.
inline std::string base64_encode( const std::string &data )
{
	return base64_encode( data.c_str(), data.size() );
}

/// @brief Decode the data from a base64 string.
inline std::string base64_decode( const std::string &data )
{
	return base64_decode( data.c_str(), data.size() );
}

////////////////////////////////////////

}

