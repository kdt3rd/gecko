//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <atomic>
#include "contract.h"

#ifdef _WIN32
# include <windows.h>
#elif defined(__MACH__)
# include <mach/mach.h>
#elif defined(__unix__)
# include <semaphore.h>
# include <cerrno>
#else
# error "Unknown platform - add implementation of semaphore"
#endif

////////////////////////////////////////

namespace base
{

/// @brief Low-level, OS level 'simple' semaphore
///
/// This is provided for completeness, but consider using the class
/// @sa semaphore below which provides lighter weight synchronization
///
class simple_semaphore
{
private:
	inline simple_semaphore( const simple_semaphore & ) = delete;
	inline simple_semaphore( simple_semaphore && ) = delete;
	inline simple_semaphore &operator=( const simple_semaphore & ) = delete;
	inline simple_semaphore &operator=( simple_semaphore && ) = delete;

public:
	inline simple_semaphore( int initCount = 0 );
	inline ~simple_semaphore( void );

	inline void wait( void );
	/// minor optimization to avoid loop setup, provide both a default single increment
	inline void signal( void );
	/// and a specific number
	inline void signal( int num );
private:

#ifdef _WIN32
	HANDLE _sem = INVALID_HANDLE_VALUE;
#elif defined(__MACH__)
	semaphore_t _sem;
#elif defined(__unix__)
	sem_t _sem;
#endif
};

///
/// @brief Class semaphore provides a ... semaphore.
///
/// This is used as a basis for some patterns where a mutex /
/// condition_variable are less desireable, for example, notifiers
/// when there work to do, but you don't want the cost of a mutex to
/// do the signalling. @sa shared_mutex, @sa auto_reset_event, and @sa event
///
/// This particular implementation uses an atomic count to provide a
/// light-weight semaphore and avoid system calls.
///
/// This is largely based on the ideas present in:
/// http://preshing.com/20150316/semaphores-are-surprisingly-versatile/
///
class semaphore
{
private:
	inline semaphore( const semaphore & ) = delete;
	inline semaphore( semaphore && ) = delete;
	inline semaphore &operator=( const semaphore & ) = delete;
	inline semaphore &operator=( semaphore && ) = delete;

public:
	/// initialize semaphore with an optional count
	//
	// since the atomic has the count, let the OS one be 0 initially
	inline semaphore( int initCount = 0 ) : _count( initCount ) {}

	/// try to wait. if the semaphore is already signalled, will attempt to decrement and return
	/// true immediately, otherwise will return false indicating the semaphore was either not signalled or another thread caused contention.
	inline bool try_wait( void )
	{
		int c = _count.load( std::memory_order_relaxed );
		return ( c > 0 && _count.compare_exchange_strong( c, c - 1, std::memory_order_acquire ) );
	}

	/// 
	/// wait for someone to signal us (or if it's already signalled, return immediately)
	inline void wait( void )
	{
		int spin_count = 10000;
		do
		{
			// try to early out while we spin
			if ( try_wait() )
				return;
			// and make sure to prevent compiler from changing loop
			std::atomic_signal_fence( std::memory_order_acquire );
		} while ( spin_count-- > 0 );
		if ( _count.fetch_sub( 1, std::memory_order_acquire ) <= 0 )
			_os_sema.wait();
	}

	/// signal the semaphore, potentially waking up num threads
	inline void signal( int num = 1 )
	{
		int c = _count.fetch_add( num, std::memory_order_release );
		c = - c;
		int nToWake = c < num ? c : num;
		if ( nToWake > 0 )
			_os_sema.signal( nToWake );
	}

private:
	std::atomic<int> _count;
	simple_semaphore _os_sema;
};

////////////////////////////////////////

inline simple_semaphore::simple_semaphore( int initCount )
{
	precondition( initCount >= 0, "Initial count for a semaphore must be positive or 0, provided: {0", initCount );
	
#ifdef _WIN32
	_sem = CreateSemaphore( NULL, initCount, MAXLONG, NULL );
#elif defined(__MACH__)
	kern_return_t r;
	if ( (r = semaphore_create( mach_task_self(), &_sem, SYNC_POLICY_FIFO, initCount )) != KERN_SUCCESS )
		throw_runtime( "Unable to initialize semaphore: {0}", mach_error_string( r ) );
#elif defined(__unix__)
	if ( sem_init( &_sem, 0, initCount ) < 0 )
		throw_errno( "Unable to initialize semaphore" );
#endif
}

////////////////////////////////////////

inline simple_semaphore::~simple_semaphore( void )
{
#ifdef _WIN32
	if ( _sem != INVALID_HANDLE_VALUE )
		CloseHandle( _sem );
#elif defined(__MACH__)
	semaphore_destroy( mach_task_self(), _sem );
#elif defined(__unix__)
	sem_destroy( &_sem );
#endif
}

////////////////////////////////////////

inline void
simple_semaphore::wait( void )
{
#ifdef _WIN32
	WaitForSingleObject( _sem, INFINITE );
#elif defined(__MACH__)
	semaphore_wait( _sem );
#elif defined(__unix__)
	int s;
	do
	{
		s = sem_wait( &_sem );
	} while ( s < 0 && errno == EINTR );
#endif
}

////////////////////////////////////////

inline void
simple_semaphore::signal( void )
{
#ifdef _WIN32
	ReleaseSemaphore( _sem, 1, NULL );
#elif defined(__MACH__)
	semaphore_signal( _sem );
#elif defined(__unix__)
	sem_post( &_sem );
#endif
}

////////////////////////////////////////

inline void
simple_semaphore::signal( int num )
{
#ifdef _WIN32
	ReleaseSemaphore( _sema, num, NULL );
#elif defined(__MACH__)
	while ( num-- > 0 )
		semaphore_signal( _sem );
#elif defined(__unix__)
	while ( num-- > 0 )
		sem_post( &_sem );
#endif
}

} // namespace base



