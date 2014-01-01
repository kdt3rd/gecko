
#pragma once

#include <platform/timer.h>

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::timer.
class timer : public platform::timer
{
public:
	/// @brief Constructor.
	timer( void );
	virtual ~timer( void );

	virtual void schedule( double seconds );
	virtual void cancel( void );

private:
	double _timeout = 0.0;
};

////////////////////////////////////////

}

