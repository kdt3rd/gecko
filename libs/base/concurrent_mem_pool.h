// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <atomic>
#include <memory>
#include <stdexcept>
#include "lock_free_list.h"

////////////////////////////////////////

namespace base
{

namespace memory_detail
{

template <size_t a>
struct aligner
{
	template <typename T>
	static inline T *calc( void *ptr )
	{
		uintptr_t pVal = reinterpret_cast<uintptr_t>( ptr );
		size_t nOut = pVal & a;
		if ( nOut != 0 )
		{
			pVal += (a - nOut);
			return reinterpret_cast<void *>( pVal );
		}
		return ptr;
	}
};

template <>
struct aligner<1>
{
	static inline void *calc( void *ptr ) { return ptr; }
};

}

template <size_t sz, size_t alignment = alignof(uint8_t), size_t chunk_size = 4096>
class concurrent_mem_pool
{
	using aligner = memory_detail::aligner<alignment>;
public:
	~concurrent_mem_pool( void )
	{
		clear();
	}

	concurrent_mem_pool( const concurrent_mem_pool & ) = delete;
	concurrent_mem_pool &operator=( const concurrent_mem_pool & ) = delete;
	concurrent_mem_pool( concurrent_mem_pool && ) = delete;
	concurrent_mem_pool &operator=( concurrent_mem_pool && ) = delete;

	template <typename T>
	T *get( size_t n )
	{
		n += alignment - 1;
		if ( block::too_large( n ) )
		{
			std::unique_ptr<uint8_t[]> data{ new uint8_t[n * sz + sizeof(big_block)] };
			big_block *bb = new (data.get()) big_block( n * sz );
			_big_blocks.push( bb );
			data.release();
			return reinterpret_cast<T *>( aligner::calc( bb->data() ) );
		}

		void *ptr = nullptr;
		bool full = false;
		// this is not so unsafe for us, since blocks are always valid
		// (well, until clear), and if it's already been moved from
		// the list to full, that's fine, the alloc will fail gracefully
		// and continue on below
		block *cur = _live.unsafe_front();
		if ( cur )
		{
			ptr = cur->alloc( n, full );
			if ( ptr )
			{
				if ( full )
					move_to_full( cur );
				return reinterpret_cast<T *>( aligner::calc( ptr ) );
			}
		}

		cur = _live.try_pop();
		if ( cur )
		{
			// the list may have changed, try again
			ptr = cur->alloc( n, full );
			if ( ptr )
				return finish<T>( cur, full, ptr );

			// hrm, couldn't allocate from this block, keep it on
			// the stack and recurse
			T *x = get<T>( n );

			return_block( cur, full );
			return x;
		}

		// fall back to 
		cur = new block;
		ptr = cur->alloc( n, full );
		return finish<T>( cur, full, ptr );
	}

	/// NB: This assumes you know that all users of any memory contained
	/// have been freed, and deletes ram without regard
	///
	/// @return std::pair
	///    first is number of bytes allocated in chunk size
	///    second is number of bytes allocated in "big" blocks (items too big to fit)
	std::pair<size_t, size_t> clear( void )
	{
		size_t nbasic = 0;
		block *cur = _live.steal();
		while ( cur )
		{
			++nbasic;
			block *old = cur;
			cur = cur->next();
			delete old;
		}

		cur = _full.steal();
		while ( cur )
		{
			++nbasic;
			block *old = cur;
			cur = cur->next();
			delete old;
		}

		size_t bytes = 0;
		big_block *bb = _big_blocks.steal();
		while ( bb )
		{
			bytes += bb->_sz;
			big_block *old = bb;
			bb = bb->next();
			delete old;
		}

		return std::make_pair( nbasic * chunk_size, bytes );
	}

private:
	struct big_block : lock_free_list_node<big_block>
	{
		big_block *_next = nullptr;
		size_t _sz = 0;
		big_block( size_t s ) : _next( nullptr ), _sz( s ) {}
		void *data() { uint8_t *raw = reinterpret_cast<uint8_t *>( this ); return raw + sizeof(big_block); }
	};

	struct block : lock_free_list_node<block>
	{
		static constexpr size_t kDataSize = chunk_size - ( sizeof(lock_free_list_node<block>) + sizeof(std::atomic<size_t>) );

		static constexpr bool too_large( size_t n ) { return n * sz >= kDataSize; }

		void *alloc( size_t n, bool &full )
		{
			size_t csz = n * sz;
			size_t x = _offset.fetch_add( csz, std::memory_order_relaxed );
			if ( ( x + csz ) > kDataSize )
			{
				_offset.fetch_sub( csz, std::memory_order_relaxed );
				full = ( kDataSize - x ) <= sz;
				return nullptr;
			}
			full = false;
			return _data + x;
		}

		std::atomic<size_t> _offset{ 0 };
		uint8_t _data[kDataSize];
	};
	static_assert( sizeof(block) == chunk_size, "Invalid data size / alignment computation" );

	inline void move_to_full( block *cur )
	{
		block *liveblocks = _live.steal( );
		block *curlive = liveblocks;
		while ( curlive )
		{
			block *next = curlive->next();
			if ( cur == curlive )
				_full.push( curlive );
			else
				_live.push( curlive );
			curlive = next;
		}
	}

	inline void return_block( block *b, bool full )
	{
		if ( full )
			_full.push( b );
		else
			_live.push( b );
	}

	template <typename T>
	inline T *finish( block *b, bool full, void *ptr )
	{
		return_block( b, full );
		return reinterpret_cast<T *>( aligner::calc( ptr ) );
	}

	lock_free_list<block> _live;
	lock_free_list<block> _full;
	lock_free_list<big_block> _big_blocks;
};

} // namespace scene

