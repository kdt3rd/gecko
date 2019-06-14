// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "dispatcher.h"

#include "event_queue.h"

#include <base/contract.h>

namespace platform
{
////////////////////////////////////////

dispatcher::dispatcher( system *s )
    : _ext_events( std::make_shared<event_queue>( s ) )
{
    // TODO: this should normally be done via add_waitable but we're
    // in the ctor, so virtual functions will be sliced...
    _waitables.push_back( _ext_events );
    _ext_events->start();
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
    for ( auto &i: _waitables )
        i->shutdown();
}

////////////////////////////////////////

void dispatcher::add_waitable( const std::shared_ptr<waitable> &w )
{
    precondition( w, "Invalid waitable to add" );
    for ( auto &cw: _waitables )
    {
        // re-registering an error? not for now
        if ( cw == w )
            return;
    }

    _waitables.push_back( w );
    w->start();
}

////////////////////////////////////////

void dispatcher::remove_waitable( const std::shared_ptr<waitable> &w )
{
    for ( auto i = _waitables.begin(); i != _waitables.end(); ++i )
    {
        if ( ( *i ) == w )
        {
            w->shutdown();
            _waitables.erase( i );
            break;
        }
    }
}

////////////////////////////////////////

} // namespace platform
