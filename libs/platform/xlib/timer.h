
#pragma once

#include <platform/timer.h>

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::timer.
class timer : public platform::timer
{
public:
	/// @brief Constructor.
	timer( void );
	~timer( void );

	void schedule( double seconds ) override;
	void cancel( void ) override;

private:
	double _timeout = 0.0;
};

////////////////////////////////////////

} }

