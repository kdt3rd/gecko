//
// Copyright (c) 2016 Kimball Thurston & Ian Godin
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include "track.h"
#include <vector>
#include <base/uri.h>
#include <functional>

namespace media
{

////////////////////////////////////////

/// container is a media container. A sample of
/// this would be a quicktime file, or an MXF,
/// or AVC, etc.
class container
{
public:
//	container( void );
//	~container( void );

	size_t size( void )
	{
		return _tracks.size();
	}

	/// There is some amount of container info
	/// such as creator and such
	void info( void ) const
	{
	}

	std::shared_ptr<track> operator[]( size_t i )
	{
		return _tracks.at( i );
	}

	std::shared_ptr<track> at( size_t i )
	{
		return _tracks.at( i );
	}

	void add_track( const std::shared_ptr<track> &t )
	{
		_tracks.push_back( t );
	}

	static container create( const base::uri &u );

	static void register_media_type( const std::string &name,
									 const std::function<container(const base::uri &)> &factory,
									 const std::vector<std::string> &extlist,
									 const std::vector<std::vector<uint8_t>> &magics = std::vector<std::vector<uint8_t>>() );
									 
private:
	static container scan_header( const base::uri &u );

	std::vector<std::shared_ptr<track>> _tracks;
};

////////////////////////////////////////

}

