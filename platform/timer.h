
#pragma once

#include <functional>

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

	/// @brief Set the callback for the timer.
	///
	/// The callback will be called once the timer is elapsed.
	/// @param f Function to call
	void when_elapsed( const std::function<void(void)> &f ) { _elapsed = f; }

	/// @brief Send a timer elapsed event.
	void elapsed( void ) { if ( _elapsed ) _elapsed(); }

private:
	std::function<void(void)> _elapsed;
};

////////////////////////////////////////

}

