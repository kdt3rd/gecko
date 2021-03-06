// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "dispatcher.h"

#include "keyboard.h"
#include "mouse.h"

#include <base/compiler_support.h>
#include <base/contract.h>
#include <base/meta.h>
#include <base/pointer.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <utf/utf.h>
#include <utf/utfcat.h>

namespace platform
{
namespace wayland
{
////////////////////////////////////////

dispatcher::dispatcher(
    ::platform::system *s, const std::shared_ptr<struct wl_display> &dpy )
    : ::platform::dispatcher( s )
    , _display( dpy )
    , _keyboard( std::make_shared<keyboard>( s ) )
    , _mouse( std::make_shared<mouse>( s ) )
{
    precondition( _display, "null display" );

    if ( pipe( _wait_pipe ) < 0 )
    {
        _wait_pipe[0] = -1;
        _wait_pipe[1] = -1;
        throw_errno( "Failed to create signaling pipe" );
    }
    if ( ::fcntl( _wait_pipe[0], F_SETFD, FD_CLOEXEC ) == -1 ||
         ::fcntl( _wait_pipe[1], F_SETFD, FD_CLOEXEC ) == -1 )
        throw_errno( "Unable to convert signaling pipe to close-on-exec" );
    if ( ::fcntl( _wait_pipe[0], F_SETFL, O_NONBLOCK ) == -1 ||
         ::fcntl( _wait_pipe[1], F_SETFL, O_NONBLOCK ) == -1 )
        throw_runtime( "Unable to convert signaling pipe to non blocking" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
    if ( _wait_pipe[0] >= 0 )
        ::close( _wait_pipe[0] );
    if ( _wait_pipe[1] >= 0 )
        ::close( _wait_pipe[1] );
}

////////////////////////////////////////

int dispatcher::execute( void )
{
    _exit_code = 0;
    _exit_requested.store( false );
#if 0
	while ( true )
	{
		waitable::time_point curt = waitable::clock::now();
		for ( auto &w: _waitables )
		{
			waitable::duration when;
			if ( w->poll_timeout( when, curt ) )
			{
				if ( when < waitable::duration::zero() )
					w->emit( curt );
				else
				{
//					std::chrono::milliseconds msecs = std::chrono::duration_cast<std::chrono::milliseconds>( when );
				}
			}
		}
		wl_display_dispatch_pending( _display.get() );
	}
#else
    fd_set                                   waitreadobjs;
    std::map<int, std::shared_ptr<waitable>> waitmap;
    std::vector<std::shared_ptr<waitable>>   firenow;
    std::vector<std::shared_ptr<waitable>>   timeouts;
    struct timeval                           tv;
    int xFD = wl_display_get_fd( _display.get() );
    while ( !_exit_requested.load() )
    {
        firenow.clear();
        waitmap.clear();
        timeouts.clear();
        int nWaits = std::max( _wait_pipe[0], xFD );
        FD_ZERO( &waitreadobjs );
        FD_SET( xFD, &waitreadobjs );
        if ( _wait_pipe[0] >= 0 )
            FD_SET( _wait_pipe[0], &waitreadobjs );

        waitable::time_point curt = waitable::clock::now();
        tv.tv_sec                 = 0;
        tv.tv_usec                = 0;
        struct timeval *tvptr     = nullptr;
        for ( auto &w: _waitables )
        {
            int oid = w->poll_object().waitable();
            if ( oid != -1 )
            {
                waitmap[oid] = w;
                FD_SET( oid, &waitreadobjs );
                nWaits = std::max( nWaits, oid );
            }
            waitable::duration when;
            if ( w->poll_timeout( when, curt ) )
            {
                if ( when < waitable::duration::zero() )
                {
                    firenow.push_back( w );
                }
                else
                {
                    timeouts.push_back( w );
                    std::chrono::seconds secs =
                        std::chrono::duration_cast<std::chrono::seconds>(
                            when );
                    when -=
                        std::chrono::duration_cast<waitable::duration>( secs );
                    std::chrono::microseconds usecs =
                        std::chrono::duration_cast<std::chrono::microseconds>(
                            when );
                    if ( secs.count() < tv.tv_sec ||
                         ( secs.count() == tv.tv_sec &&
                           usecs.count() < tv.tv_usec ) )
                    {
                        tv.tv_sec  = secs.count();
                        tv.tv_usec = usecs.count();
                    }

                    tvptr = &tv;
                }
            }
        }

        bool doTimeouts = false;
        if ( firenow.empty() )
        {
            ++nWaits;
            wl_display_flush( _display.get() );
            int selrv = select( nWaits, &waitreadobjs, nullptr, nullptr, tvptr );

            // timeout if selrv == 0
            if ( selrv < 0 )
                throw_errno( "Error waiting for events to be available" );

            doTimeouts = ( selrv == 0 );
            for ( auto &x: waitmap )
            {
                if ( FD_ISSET( x.first, &waitreadobjs ) )
                    firenow.push_back( x.second );
            }
            if ( FD_ISSET( _wait_pipe[0], &waitreadobjs ) )
            {
                char j;
                int  rc;
                do
                {
                    rc = ::read( _wait_pipe[0], &j, 1 );
                } while ( rc > 0 || ( rc == -1 && errno == EINTR ) );
            }
            if ( FD_ISSET( xFD, &waitreadobjs ) )
                wl_display_dispatch( _display.get() );
        }

        curt = waitable::clock::now();
        if ( doTimeouts )
        {
            for ( auto &w: timeouts )
                w->emit( curt );
        }
        for ( auto &w: firenow )
            w->emit( curt );
        if ( drain_wayland_events() )
            break;
    }
#endif
    return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
    _exit_code = code;
    _exit_requested.store( true );
}

////////////////////////////////////////

void dispatcher::add_waitable( const std::shared_ptr<waitable> &w )
{
    ::platform::dispatcher::add_waitable( w );
    wake_up_executor();
}

////////////////////////////////////////

void dispatcher::remove_waitable( const std::shared_ptr<waitable> &w )
{
    ::platform::dispatcher::remove_waitable( w );
    wake_up_executor();
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
    //	_windows[w->id()] = w;
}

////////////////////////////////////////

void dispatcher::remove_window( const std::shared_ptr<window> &w ) {}

////////////////////////////////////////

void dispatcher::wake_up_executor( void )
{
    if ( _wait_pipe[1] >= 0 )
    {
        char x = 'x';
        ::write( _wait_pipe[1], &x, sizeof( char ) );
    }
}

////////////////////////////////////////

bool dispatcher::drain_wayland_events( void )
{
    bool done = false;
    TODO( "How do we know when we're finished?" )
    wl_display_dispatch_pending( _display.get() );
    return done;
}

////////////////////////////////////////

} // namespace wayland
} // namespace platform
