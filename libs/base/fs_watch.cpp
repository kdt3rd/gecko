// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "fs_watch.h"

#include "utility.h"

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

fs_watcher::fs_watcher( void ) {}

////////////////////////////////////////

fs_watcher::~fs_watcher( void ) {}

////////////////////////////////////////

fs_watch::fs_watch(
    const std::shared_ptr<fs_watcher> &fsw, const event_handler &evt )
    : _fsw( fsw ), _evt_cb( evt )
{}

////////////////////////////////////////

fs_watch::~fs_watch( void )
{
    if ( _fsw )
        _fsw->unregisterWatch( *this );
}

////////////////////////////////////////

fs_watch::fs_watch( fs_watch &&o )
    : _fsw( exchange( o._fsw, std::shared_ptr<fs_watcher>() ) )
    , _evt_cb( std::move( o._evt_cb ) )
{
    _fsw->move( o, *this );
}

////////////////////////////////////////

fs_watch &fs_watch::operator=( fs_watch &&o )
{
    _fsw    = exchange( o._fsw, std::shared_ptr<fs_watcher>() );
    _evt_cb = std::move( o._evt_cb );
    _fsw->move( o, *this );
    return *this;
}

////////////////////////////////////////

} // namespace base
