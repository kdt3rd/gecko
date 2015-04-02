
#pragma once

#include "track.h"
#include <vector>

namespace media
{

////////////////////////////////////////

class container
{
public:
	container( const std::function<void(container&)> &upd );
	~container( void );

	size_t size( void )
	{
		update();
		return _tracks.size();
	}

	std::shared_ptr<track> operator[]( size_t i )
	{
		update();
		return _tracks.at( i );
	}

	std::shared_ptr<track> at( size_t i )
	{
		update();
		return _tracks.at( i );
	}

	void add_track( const std::shared_ptr<track> &t )
	{
		_tracks.push_back( t );
	}

private:
	void update( void );

	bool _outofdate = true;
	std::function<void(container&)> _update;
	std::vector<std::shared_ptr<track>> _tracks;
};

////////////////////////////////////////

}

