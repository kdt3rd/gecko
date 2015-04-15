
#pragma once

#include "track.h"
#include <vector>

namespace media
{

////////////////////////////////////////

class container
{
public:
	container( void );
	~container( void );

	size_t size( void )
	{
		return _tracks.size();
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

private:

	std::vector<std::shared_ptr<track>> _tracks;
};

////////////////////////////////////////

}

