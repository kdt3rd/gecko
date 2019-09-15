// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "event_queue.h"

#include "event_target.h"

////////////////////////////////////////

namespace platform
{
////////////////////////////////////////

event_queue::event_queue( system *s )
    : waitable( s ), _signalled( false ), _pipe( true, false )
{}

////////////////////////////////////////

event_queue::~event_queue( void ) {}

////////////////////////////////////////

void event_queue::start( void ) {}

////////////////////////////////////////

void event_queue::cancel( void ) {}

////////////////////////////////////////

void event_queue::shutdown( void ) {}

////////////////////////////////////////

waitable::wait event_queue::poll_object( void ) { return _pipe.readable(); }

////////////////////////////////////////

bool event_queue::poll_timeout( duration &when, const time_point &curtime )
{
    return false;
}

////////////////////////////////////////

void event_queue::emit( const time_point &curtime )
{
    if ( _grabber )
        drain( _grabber );
}

////////////////////////////////////////

void event_queue::queue(
    const event &e, const std::shared_ptr<event_target> &targ )
{
    std::lock_guard<std::mutex> lk( _mutex );
    _events.emplace_back( std::make_pair( targ, e ) );
    if ( !_signalled )
    {
        _signalled = true;
        _pipe.write( "e", 1 );
    }
}

////////////////////////////////////////

void event_queue::drain( const std::shared_ptr<event_target> &et )
{
    std::lock_guard<std::mutex> lk( _mutex );

    while ( !_events.empty() )
    {
        auto &cur = _events.front();
        if ( cur.first )
            cur.first->process_event( cur.second );
        else if ( et )
            et->process_event( cur.second );
        _events.pop_front();
    }

    if ( _signalled )
    {
        char buf;
        _pipe.read( &buf, 1 );
        _signalled = false;
    }
}

} // namespace platform
