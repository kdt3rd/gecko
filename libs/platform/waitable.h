// Copyright (c) 2016-2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>
#include <chrono>
#include "event_source.h"
#include <base/wait.h>

////////////////////////////////////////

namespace platform
{

class system;

///
/// @brief Class waitable provides an abstract class used for waiting
/// for multiple event sources.
///
/// The idea is that all event sources, such as a file handle for the
/// UI events as in X11, a (periodic/one-off) timer, a file handle
/// that corresponds to a USB device, or a recording of previous
/// events, derive from this base class, and then the system event
/// dispatcher has a list of these to be able to control the central
/// 'wait' loop.
class waitable : public event_source
{
public:
	using clock = std::chrono::high_resolution_clock;
	using time_point = clock::time_point;
	using duration = clock::duration;
	// TODO: should we just use base::wait, or just the typedefs?
	using wait = base::wait;
	static const wait::wait_type INVALID_WAIT = base::wait::INVALID_WAIT;

	waitable( system *s );
	virtual ~waitable( void );

	/// This can be called by anyone to cancel this waitable as a
	/// source. This may trigger the dispatcher to wake up.  This may
	/// only be used by USB devices and similar when they are
	/// unplugged, but other uses are possible.
	///
	/// This should be thread safe as necessary
	virtual void cancel( void ) = 0;

	/// @brief Used to query waitable object
	///
	/// This will be interpreted differently depending on the
	/// underlying system in place, but is meant to be the same things
	/// that are used for select or WaitForSingleObject, etc. see
	/// base/wait.h for more
	///
	/// if not valid, should be wait::INVALID_WAIT;
	///
	/// This should be thread safe as necessary
	virtual wait poll_object( void ) = 0; 

	/// @brief Used to query amount of time to wait between
	///
	/// This should return true if it has a timeout value, and set the
	/// duration appropriately, indicating that any waiting should
	/// terminate in the duration.
	///
	/// if this returns true and when is duration::zero(), the waitable will be
	/// considered expired and will trigger immediately
	///
	/// This does not have to be used solely by timer events, but can
	/// be used for periodic polling of a device that does not have a
	/// blocking wait mechanism.
	///
	/// This should be thread safe as necessary
	virtual bool poll_timeout( duration &when, const time_point &curtime ) = 0;


	/// @brief emit any events as appropriate.
	///
	/// for timers, they should check the current time for expiry
	virtual void emit( const time_point &curtime ) = 0;

private:
	// not copyable to suggest that people use
	// a shared_ptr and register it appropriately
	// with the dispatcher
	waitable( const waitable & ) = delete;
	waitable( waitable && ) = delete;
	waitable &operator=( const waitable & ) = delete;
	waitable &operator=( waitable && ) = delete;
};

} // namespace platform



