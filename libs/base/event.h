// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "semaphore.h"

#include <atomic>
#include <condition_variable>
#include <mutex>

////////////////////////////////////////

namespace base
{
///
/// @brief Class event provides a basic event synchronization object.
///
/// This is similar
///
class event
{
public:
    /// construct the event with the initial state
    event( bool initState = false );
    ~event( void );

    /// Waits for the event to be raised
    ///
    /// TODO: consider adding a timed wait? unclear with the semantics
    /// of an event remaining signalled until it is lowered whether a
    /// wait_until routine ever makes sense, but a wait_for may.
    void wait( void );

    /// Behaves the same as SetEvent under WIN32 - triggers the event and
    /// wakes any waiting threads
    void raise( void );

    /// Behaves the same as ResetEvent under WIN32 - resets the event, the
    /// next time a wait happens, the thread calling wait will block until
    /// the event is raised again
    void lower( void );

    /// TBD: May be used to implement a wait on multiple objects
    void *native_handle( void );

private:
#ifdef _WIN32
    HANDLE _event;
#else
    // could use eventfd if we want to be able to wait on this in the future...
    std::mutex              _mutex;
    std::condition_variable _cond;
    bool                    _state;
#endif
};

/// @brief an auto-reset event synchronizer to notify another thread of an event
///
/// This allows producers to always, in a light-weight manner, signal
/// that there's something to do without having to incur a lock, and
/// only incur a system call to wake when there's actually a thread
/// waiting.  This is commonly used to notify a single background
/// thread that there is something to do.
///
/// Based on:
/// http://preshing.com/20150316/semaphores-are-surprisingly-versatile/
class auto_reset_event
{
private:
    inline auto_reset_event( const auto_reset_event & ) = delete;
    inline auto_reset_event( auto_reset_event && )      = delete;
    inline auto_reset_event &operator=( const auto_reset_event & ) = delete;
    inline auto_reset_event &operator=( auto_reset_event && ) = delete;

public:
    auto_reset_event( bool initState = false ) : _state( initState ? 1 : 0 ) {}

    inline void set( void )
    {
        int s = _state.load( std::memory_order_relaxed );
        while ( true )
        {
            int ns = std::min( s + 1, int( 1 ) );
            if ( _state.compare_exchange_weak(
                     s,
                     ns,
                     std::memory_order_release,
                     std::memory_order_relaxed ) )
                break;
            // compare / exchange failed, try again...
        }
        if ( s < 0 )
            _sem.signal();
    }

    inline void wait( void )
    {
        int s = _state.fetch_sub( 1, std::memory_order_acquire );
        if ( s < 1 )
            _sem.wait();
    }

private:
    std::atomic<int> _state;
    semaphore        _sem;
};

} // namespace base
