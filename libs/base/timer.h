
#pragma once

#include <chrono>

namespace base
{

////////////////////////////////////////

class timer
{
public:
	explicit timer( bool run = false )
	{
		if ( run )
			start();
	}

	void start( void )
	{
		_start = std::chrono::high_resolution_clock::now();
	}

	std::chrono::milliseconds elapsed( void ) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - _start );
	}

private:
	std::chrono::high_resolution_clock::time_point _start;
};

////////////////////////////////////////

}

