
#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <initializer_list>

namespace core
{

////////////////////////////////////////

typedef std::tuple<const char *,const char *,size_t> resource;

////////////////////////////////////////

class data_resource
{
public:
	data_resource( std::initializer_list<resource> data )
		: _data( data )
	{
	}

	std::string operator()( const std::string &filename )
	{
		for ( auto t: _data )
		{
			if ( filename == std::get<0>( t ) )
				return std::string( std::get<1>( t ), std::get<2>( t ) );
		}
		throw std::runtime_error( "resource file not found: " + filename );
	}

private:
	std::vector<resource> _data;
};

////////////////////////////////////////

}

