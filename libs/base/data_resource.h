//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <initializer_list>
#include "contract.h"

namespace base
{

////////////////////////////////////////

/// @brief A resource made of a filename, data, and a size
typedef std::tuple<const char *,const char *,size_t> resource;

////////////////////////////////////////

/// @brief A resource containing one or more files
class data_resource
{
public:
	/// @brief Construct a resource from the given list
	data_resource( std::initializer_list<resource> data )
		: _data( data )
	{
	}

	/// @brief Access the resource.
	std::string operator()( const std::string &filename )
	{
		/// TODO This will copy the data... try to find a better way...
		for ( auto t: _data )
		{
			if ( filename == std::get<0>( t ) )
				return std::string( std::get<1>( t ), std::get<2>( t ) );
		}
		throw_runtime( "resource file not found {0}", filename );
	}

private:
	std::vector<resource> _data;
};

////////////////////////////////////////

}

