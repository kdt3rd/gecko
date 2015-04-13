
#pragma once

#include <base/uri.h>

namespace media
{

////////////////////////////////////////

class file_sequence
{
public:
	file_sequence( base::uri path );

	const base::uri &uri( void ) const
	{
		return _uri;
	}

	base::uri get_frame( int64_t f );

	bool extract_frame( const base::uri &u, int64_t &f );

private:
	size_t _pattern_start;
	size_t _pattern_size;
	size_t _pattern_rest;
	base::uri _uri;
};

////////////////////////////////////////

}

