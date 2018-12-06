//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
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

////////////////////////////////////////

class image_frame : public sample_data
{
public:
	image_frame( int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
		: _area( area_rect::from_points( x1, y1, x2, y2 ) )
	{
	}
	virtual ~image_frame( void );

	virtual size_t item_count( void ) const { return 1; }

	size_t add_channel( std::string n, image_buffer i )
	{
		precondition( i.width() == width(), "invalid channel width {0} vs {1}", i.width(), width() );
		precondition( i.height() == height(), "invalid channel height {0} vs {1}", i.height(), height() );
		_names[std::move(n)] = _channels.size();
		_channels.emplace_back( std::move( i ) );
		return _channels.size() - 1;
	}

	size_t add_channel( image_buffer i )
	{
		precondition( i.width() == width(), "invalid channel width {0} vs {1}", i.width(), width() );
		precondition( i.height() == height(), "invalid channel height {0} vs {1}", i.height(), height() );
		_channels.emplace_back( std::move( i ) );
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

	bool is_interleaved( void ) const
	{
		size_t N = size();
		// consider single channel image not interleaved...
		if ( N < 2 )
			return false;

		const std::shared_ptr<void> &fcb = _channels.front().raw();
		for ( size_t i = 1; i != N; ++i )
		{
			if ( _channels[i].raw() != fcb )
				return false;
		}
		return true;
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

