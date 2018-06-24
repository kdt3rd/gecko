//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "event.h"
#include "waitable.h"
#include <base/pipe.h>
#include <mutex>
#include <deque>

////////////////////////////////////////

namespace platform
{

class system;

///
/// @brief Class event_queue provides a thread-safe queue of events
///
/// This can be waited upon by the dispatcher.
///
/// The primary purpose of this is to be used by the dispatcher and
/// event sources to provide extraneous / background thread /
/// cross-thread events. It is expected that the dispatcher will
/// "grab" the event queue with whatever current window, or with the
/// last one such that events are actually delivered to somewhere,
/// otherwise they will sit until an explicit drain call is requested
///
/// NB: This is not really designed for super-high concurrency, in
/// that it uses a simple mutex and deque of events, and queue will be
/// blocked while draining, and vice-versa
///
class event_queue : public waitable
{
public:
	event_queue( system *s );
	virtual ~event_queue( void );

	void start( void ) final;
	void cancel( void ) final;
	void shutdown( void ) final;

	wait poll_object( void ) final;
	bool poll_timeout( duration &when, const time_point &curtime ) final;

	void emit( const time_point &curtime ) final;

	void queue( const event &e, const std::shared_ptr<event_target> &targ = std::shared_ptr<event_target>() );
	void drain( const std::shared_ptr<event_target> &et );

private:
	std::mutex _mutex;

	std::deque< std::pair<std::shared_ptr<event_target>, event> > _events;
	bool _signalled = false;
	base::pipe _pipe;
};

} // namespace platform



