// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "event.h"

#include "contract.h"

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

event::event( bool initState )
#ifdef WIN32
    : _event( CreateEvent( NULL, false, BOOL( initState ), NULL ) )
{
    if ( _event == INVALID_HANDLE_VALUE )
        throw_lasterror( "Unable to create event object" );
}
#else
    : _state( initState )
{}
#endif

////////////////////////////////////////

event::~event( void )
{
#ifdef WIN32
    if ( _event != INVALID_HANDLE_VALUE )
        CloseHandle( _event );
#endif
}

////////////////////////////////////////

void event::wait( void )
{
#ifdef WIN32
    WaitForSingleObject( _event, INFINITE );
#else
    std::unique_lock<std::mutex> lk( _mutex );
    if ( !_state )
        _cond.wait( lk );
#endif
}

////////////////////////////////////////

void event::raise( void )
{
#ifdef WIN32
    SetEvent( _event );
#else
    std::lock_guard<std::mutex> lk( _mutex );
    _state = true;
    _cond.notify_all();
#endif
}

////////////////////////////////////////

void event::lower( void )
{
#ifdef WIN32
    ResetEvent( _event );
#else
    std::lock_guard<std::mutex> lk( _mutex );
    _state = false;
#endif
}

////////////////////////////////////////

void *event::native_handle( void )
{
#ifdef _WIN32
    return _event;
#else
    return this;
#endif
}

////////////////////////////////////////

} // namespace base
