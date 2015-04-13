
#pragma once

#include "image_buffer.h"

#include <map>
#include <vector>

namespace media
{

////////////////////////////////////////

class image_frame
{
public:
	image_frame( void )
	{
	}

	size_t add_channel( std::string n, image_buffer &i )
	{
		_names[std::move(n)] = _channels.size();
		_channels.push_back( i );
	}

	size_t add_channel( image_buffer &i )
	{
		_channels.push_back( i );
	}

	size_t add_channel( image_buffer &&i )
	{
		_channels.push_back( std::move( i ) );
	}

	void add_name( size_t chan, std::string name )
	{
		_names[std::move(name)] = chan;
	}

	size_t size( void ) const
	{
		return _channels.size();
	}

	const image_buffer &operator[]( size_t i ) const
	{
		return _channels.at( i );
	}

	const image_buffer &at( size_t i ) const
	{
		return _channels.at( i );
	}

	const image_buffer &operator[]( const std::string &n ) const
	{
		return _channels.at( _names.at( n ) );
	}

	const image_buffer &at( const std::string &n ) const
	{
		return _channels.at( _names.at( n ) );
	}

	size_t channel( const std::string &n ) const
	{
		return _names.at( n );
	}

	std::string name( size_t i ) const
	{
		for ( auto &n: _names )
		{
			if ( n.second == i )
				return n.first;
		}
		return std::string();
	}

private:
	std::map<std::string,size_t> _names;
	std::vector<image_buffer> _channels;
};

////////////////////////////////////////

}

