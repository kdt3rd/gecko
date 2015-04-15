
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
	image_frame( int64_t w, int64_t h )
		: _width( w ), _height( h )
	{
	}

	size_t add_channel( std::string n, image_buffer &i )
	{
		precondition( i.width() == _width, "invalid channel width" );
		precondition( i.height() == _height, "invalid channel height" );
		_names[std::move(n)] = _channels.size();
		_channels.push_back( i );
	}

	size_t add_channel( image_buffer &i )
	{
		precondition( i.width() == _width, "invalid channel width" );
		precondition( i.height() == _height, "invalid channel height" );
		_channels.push_back( i );
	}

	size_t add_channel( image_buffer &&i )
	{
		precondition( i.width() == _width, "invalid channel width" );
		precondition( i.height() == _height, "invalid channel height" );
		_channels.push_back( std::move( i ) );
	}

	void add_name( size_t chan, std::string name )
	{
		_names[std::move(name)] = chan;
	}

	int64_t width( void ) const
	{
		return _width;
	}

	int64_t height( void ) const
	{
		return _height;
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

	template<typename ...Channels>
	bool has_channels( const char *c, Channels ...channels )
	{
		return has_channel( c ) && has_channels( channels... );
	}

	bool has_channel( const char *c )
	{
		return _names.find( c ) != _names.end();
	}

private:
	bool has_channels( void )
	{
		return true;
	}

	int64_t _width;
	int64_t _height;
	std::map<std::string,size_t> _names;
	std::vector<image_buffer> _channels;
};

////////////////////////////////////////

}

