//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
	intptr_t poll_object( void ) override;
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

