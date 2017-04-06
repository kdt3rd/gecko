//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <chrono>

namespace base
{

////////////////////////////////////////

/// @brief Timer class
///
/// Timer with millisecond accuracy.
class timer
{
public:
	/// @brief Default constructor
	///
	/// If run is true, the timer is start after construction.
	explicit timer( bool run = false )
	{
		if ( run )
			start();
	}

	/// @brief Start the timer
	///
	/// This can also restart the timer
	void start( void )
	{
		_start = std::chrono::high_resolution_clock::now();
	}

	/// @brief The number of milliseconds elapsed since last start
	///
	/// Use elapsed().count() to get an integer of the elapsed time.
	std::chrono::milliseconds elapsed( void ) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - _start );
	}

private:
	std::chrono::high_resolution_clock::time_point _start;
};

////////////////////////////////////////

}

