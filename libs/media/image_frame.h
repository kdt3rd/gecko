//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/contract.h>

#include "sample_data.h"
#include "image_buffer.h"
#include "metadata.h"

#include <map>
#include <vector>

namespace media
{

class area_rect
{
public:
	constexpr area_rect( void ) noexcept = default;
	area_rect( int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
			: _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 )
	{
		precondition( x2 >= x1, "Invalid x coordinate, x2 ({0}) >= x1({1})", x2, x1 );
		precondition( y2 >= y1, "Invalid y coordinate, y2 ({0}) >= y1({1})", y2, y1 );
	}
	~area_rect( void ) = default;

	constexpr bool valid( void ) const { return _x2 >= _x1; }
	constexpr int64_t x1( void ) const { return _x1; }
	constexpr int64_t y1( void ) const { return _y1; }
	constexpr int64_t x2( void ) const { return _x2; }
	constexpr int64_t y2( void ) const { return _y2; }

	constexpr int64_t width( void ) const
	{
		return _x2 - _x1 + 1;
	}

	constexpr int64_t height( void ) const
	{
		return _y2 - _y1 + 1;
	}
private:
	int64_t _x1 = 0;
	int64_t _y1 = 0;
	int64_t _x2 = -1;
	int64_t _y2 = -1;
};

////////////////////////////////////////

class image_frame : public sample_data
{
public:
	image_frame( int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
			: _area( x1, y1, x2, y2 )
	{
	}
	virtual ~image_frame( void );

	virtual size_t item_count( void ) const { return 1; }

	size_t add_channel( std::string n, image_buffer &i )
	{
		precondition( i.width() == width(), "invalid channel width {0} vs {1}", i.width(), width() );
		precondition( i.height() == height(), "invalid channel height {0} vs {1}", i.height(), height() );
		_names[std::move(n)] = _channels.size();
		_channels.push_back( i );
		return _channels.size() - 1;
	}

	size_t add_channel( image_buffer &i )
	{
		precondition( i.width() == width(), "invalid channel width {0} vs {1}", i.width(), width() );
		precondition( i.height() == height(), "invalid channel height {0} vs {1}", i.height(), height() );
		_channels.push_back( i );
		return _channels.size() - 1;
	}

	size_t add_channel( image_buffer &&i )
	{
		precondition( i.width() == width(), "invalid channel width {0} vs {1}", i.width(), width() );
		precondition( i.height() == height(), "invalid channel height {0} vs {1}", i.height(), height() );
		_channels.push_back( std::move( i ) );
		return _channels.size() - 1;
	}

	void add_name( size_t chan, std::string name )
	{
		_names[std::move(name)] = chan;
	}

	int64_t x1( void ) const { return _area.x1(); }
	int64_t y1( void ) const { return _area.y1(); }
	int64_t x2( void ) const { return _area.x2(); }
	int64_t y2( void ) const { return _area.y2(); }

	int64_t width( void ) const
	{
		return _area.width();
	}

	int64_t height( void ) const
	{
		return _area.height();
	}

	size_t size( void ) const
	{
		return _channels.size();
	}

	image_buffer &operator[]( size_t i )
	{
		return _channels.at( i );
	}
	const image_buffer &operator[]( size_t i ) const
	{
		return _channels.at( i );
	}

	image_buffer &at( size_t i )
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
	bool has_channels( const char *c, Channels ...channels ) const
	{
		return has_channel( c ) && has_channels( channels... );
	}

	bool has_channel( const char *c ) const
	{
		return _names.find( c ) != _names.end();
	}

	bool has_channel( const std::string &n ) const
	{
		return _names.find( n ) != _names.end();
	}

	inline void set_meta( base::cstring name, metadata_value v ) { _metadata[name] = std::move( v ); }
	inline const metadata &meta( void ) const { return _metadata; }
	void copy_meta( const metadata &m );

	inline void set_render_data( base::cstring name, metadata_value v ) { _renderdata[name] = std::move( v ); }
	inline const metadata &render_data( void ) const { return _renderdata; }

private:
	bool has_channels( void )
	{
		return true;
	}

	area_rect _area;
	std::map<std::string,size_t> _names;
	std::vector<image_buffer> _channels;
	metadata _metadata;
	metadata _renderdata;
};

////////////////////////////////////////

}

