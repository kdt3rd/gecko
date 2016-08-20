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
#include <cstdint>
#include <chrono>

////////////////////////////////////////

namespace platform
{

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
class waitable
{
public:
	typedef std::chrono::high_resolution_clock clock;
	typedef clock::time_point time_point;
	typedef clock::duration duration;

	waitable( void );
	virtual ~waitable( void );

	/// @brief start will be called by the dispatcher when this
	/// waitable is finished being registered.
	///
	/// This should be thread safe as necessary
	virtual void start( void ) = 0;

	/// This can be called by anyone to cancel this waitable as a
	/// source. This may trigger the dispatcher to wake up.  This may
	/// only be used by USB devices and similar when they are
	/// unplugged, but other uses are possible.
	///
	/// This should be thread safe as necessary
	virtual void cancel( void ) = 0;

	/// This will be called by the dispatcher when it is exiting to
	/// allow the waitable to perform any cleanup as necessary
	///
	/// This should be thread safe as necessary
	virtual void shutdown( void ) = 0;

	/// @brief Used to query waitable object
	///
	/// This will be interpreted differently depending on the underlying system in place:
	///
	/// Unix: this should be a file handle (int), or intptr_t(-1) if not file handle
	/// OS/X: TODO: cocoa also a file handle? or an NSObject for a run loop source?
	/// Windows: this should be a HANDLE, or INVALID_HANDLE_VALUE if not valid
	///
	/// This should be thread safe as necessary
	virtual intptr_t poll_object( void ) = 0; 

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



