
#pragma once

#include <platform/timer.h>

namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of timer.
class timer : public platform::timer
{
public:
	timer( void );
	~timer( void );

	void schedule( double seconds ) override;
	void cancel( void ) override;

private:
	double _timeout = 0.0;
};

////////////////////////////////////////

}

