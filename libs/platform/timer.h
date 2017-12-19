//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <functional>
#include <chrono>
#include "waitable.h"

namespace platform
{

////////////////////////////////////////

/// @brief Timer 'device'.
///
/// A timer device which can send out events after a fixed period of
/// time, and either repeat or just be a one-off.
///
/// @TODO determine thread safety requirements
class timer : public waitable
{
public:
	typedef std::function<void(void)> function;
	using waitable::clock;
	using waitable::time_point;
	using waitable::duration;

	timer( void );
	/// @brief Destructor.
	virtual ~timer( void );

	/// @brief activates (continues) a timer
	///
	/// if the timer has elapsed since it was paused, it will trigger
	/// the next time through the event loop unless a reset is called
	void activate( bool on );

	/// @brief resets to one_off timer expiring in given seconds
	///
	/// NB: this does not change the pause state
	void reset_one_off( double in_seconds );
	/// @brief resets to one_off timer expiring in given seconds
	///
	/// NB: this does not change the pause state
	void reset_one_off( duration in_seconds );
	/// @brief resets to one_off timer expire at given time (one-off)
	///
	/// NB: this does not change the pause state
	void reset_one_off( time_point when );

	/// @brief resets repeat to duration seconds from now
	///
	/// NB: this does not change the pause state
	void reset_repeat( double dur_sec );

	/// @brief resets repeat to duration seconds from now
	///
	/// NB: this does not change the pause state
	void reset_repeat( duration dur );

	/// @brief pauses the timer
	/// @sa waitable::start
	void start( void ) override;
	/// @sa waitable::cancel
	void cancel( void ) override;
	/// @sa waitable::shutdown
	void shutdown( void ) override;
	/// @sa waitable::poll_object
	wait poll_object( void ) override;
	/// @sa waitable::poll_timeout
	bool poll_timeout( duration &when, const time_point &curtime ) override;
	/// @sa waitable::emit
	void emit( const time_point &curtime ) override;

	/// @brief Callback for the timer.
	///
	/// The callback will be called once the timer is elapsed.
	function elapsed;

private:
	// not copyable to suggest that people use
	// a shared_ptr and register it appropriately
	// with the dispatcher
	timer( const timer & ) = delete;
	timer( timer && ) = delete;
	timer &operator=( const timer & ) = delete;
	timer &operator=( timer && ) = delete;

	time_point _expire_time;
	duration _repeat_dur = duration::zero();
	bool _active = false;
};


////////////////////////////////////////

}

