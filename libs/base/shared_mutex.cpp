// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "shared_mutex.h"
#include "contract.h"

////////////////////////////////////////

namespace
{

static const uint64_t kReaderBits = 22;
static const uint64_t kWriterBits = 22;
static const uint64_t kReadWriterBits = 20;
static_assert( kReadWriterBits + kReaderBits + kWriterBits <= 64, "Invalid number of bits for atomic store" );
static const uint64_t kReaderOffset = 0;
static const uint64_t kWriterOffset = kReaderOffset + kReaderBits;
static const uint64_t kReadWriterOffset = kWriterOffset + kWriterBits;

static const uint64_t kMaxWriterCount = (uint64_t(1) << kWriterBits) - 1;
static const uint64_t kMaxReaderCount = (uint64_t(1) << kReaderBits) - 1;
static const uint64_t kMaxReadWriterCount = (uint64_t(1) << kReadWriterBits) - 1;

//static const uint64_t kReaderMask = kMaxReaderCount << kReaderOffset;
static const uint64_t kWriterMask = kMaxWriterCount << kWriterOffset;
//static const uint64_t kReadWriterMask = kMaxReadWriterCount << kReadWriterOffset;

static const uint64_t kOneWriter = uint64_t(1) << kWriterOffset;
static const uint64_t kOneReader = uint64_t(1) << kReaderOffset;
static const uint64_t kOneReadWriter = uint64_t(1) << kReadWriterOffset;

inline uint64_t numReaders( uint64_t v ) { return ( v >> kReaderOffset ) & kMaxReaderCount; }
inline uint64_t numWriters( uint64_t v ) { return ( v >> kWriterOffset ) & kMaxWriterCount; }
inline uint64_t numReadWriters( uint64_t v ) { return ( v >> kReadWriterOffset ) & kMaxReadWriterCount; }

inline uint64_t incReader( uint64_t v )
{
	precondition( numReaders( v + kOneReader ) < kMaxReaderCount, "Too many readers for shared_mutex" );
	return v + kOneReader;
}

inline uint64_t swapReadWriters( uint64_t v )
{
	return ( ( ( ( v >> kReadWriterOffset ) & kMaxReadWriterCount ) << kReaderOffset ) |
			 ( v & kWriterMask ) );
}

inline uint64_t decWriter( uint64_t v )
{
	precondition( numWriters( v ) > 0, "Attempt to decrement empty reader pool in shared_mutex" );
	return v - kOneWriter;
}

inline uint64_t incReadWriter( uint64_t v )
{
	precondition( numReadWriters( v + kOneReadWriter ) <= kMaxReadWriterCount, "Too many readers for shared_mutex" );
	return v + kOneReadWriter;
}

} // empty namespace

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

shared_mutex::shared_mutex( void )
	: _state( 0UL )
{
}

////////////////////////////////////////

shared_mutex::~shared_mutex( void )
{
}

////////////////////////////////////////

void
shared_mutex::lock( void )
{
	uint64_t curState = _state.fetch_add( kOneWriter, std::memory_order_acquire );
	if ( numReaders( curState ) > 0 || numWriters( curState ) > 0 )
		_write_sem.wait();
}

////////////////////////////////////////

bool
shared_mutex::try_lock( void )
{
	uint64_t c = _state.load( std::memory_order_relaxed );
	return ( c == 0UL && _state.compare_exchange_strong( c, c + kOneWriter, std::memory_order_acquire ) );
}

////////////////////////////////////////

void
shared_mutex::unlock( void )
{
	uint64_t curState = _state.load( std::memory_order_relaxed );
	uint64_t newState;
	uint64_t waitToRead = 0;
	do
	{
		precondition( numReaders( curState ) == 0, "Expecting 0 readers during unique write lock" );
		newState = decWriter( curState );
		waitToRead = numReadWriters( curState );
		if ( waitToRead > 0 )
			newState = swapReadWriters( newState );
	} while ( ! _state.compare_exchange_weak( curState, newState, std::memory_order_release, std::memory_order_relaxed ) );

	if ( waitToRead > 0 )
		_read_sem.signal( static_cast<int>( waitToRead ) );
	else if ( numWriters( curState ) > 1 )
		_write_sem.signal();
}

////////////////////////////////////////

void
shared_mutex::lock_shared( void )
{
	uint64_t prevState = _state.load( std::memory_order_relaxed );
	uint64_t newState;
	uint64_t numW;
	do
	{
		numW = numWriters( prevState );
		if ( numW > 0 )
			newState = incReadWriter( prevState );
		else
			newState = incReader( prevState );
	} while ( ! _state.compare_exchange_weak( prevState, newState, std::memory_order_release, std::memory_order_relaxed ) );

	if ( numW > 0 )
		_read_sem.wait();
}

////////////////////////////////////////

bool
shared_mutex::try_lock_shared( void )
{
	uint64_t c = _state.load( std::memory_order_relaxed );
	return ( numWriters( c ) == 0UL && _state.compare_exchange_strong( c, c + kOneReader, std::memory_order_acquire ) );
}

////////////////////////////////////////

void
shared_mutex::unlock_shared( void )
{
	uint64_t prevState = _state.fetch_sub( kOneReader, std::memory_order_release );
	precondition( numReaders( prevState ) > 0, "Attempt to unlock_shared a shared_mutex with no read locks" );
	if ( numReaders( prevState ) == 1 && numWriters( prevState ) > 0 )
		_write_sem.signal();
}

////////////////////////////////////////

} // base
