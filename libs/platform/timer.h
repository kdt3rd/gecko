
#pragma once

#include <base/action.h>

namespace platform
{

////////////////////////////////////////

/// @brief Timer device.
///
/// A timer device which can send out events after a fixed period of time.
class timer
{
public:
	/// @brief Constructor.
	timer( void );

	/// @brief Destructor.
	virtual ~timer( void );

	/// @brief Schedule a timer event.
	///
	/// Schedule a single timer event to occur in the given amount of time.
	/// @param seconds How long before the event is sent
	virtual void schedule( double seconds ) = 0;

	/// @brief Cancel the timer.
	virtual void cancel( void ) = 0;

	/// @brief Callback for the timer.
	///
	/// The callback will be called once the timer is elapsed.
	action<void( void )> elapsed;
};

////////////////////////////////////////

}

