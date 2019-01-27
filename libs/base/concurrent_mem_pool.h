// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <atomic>
#include <memory>
#include <stdexcept>

////////////////////////////////////////

namespace base
{

template <size_t sz, size_t alignment = alignof(uint8_t), size_t chunk_size = 4096>
class concurrent_mem_pool
{
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
		if ( block::too_large( n ) )
		{
			std::unique_ptr<uint8_t[]> data{ new uint8_t[n * sz + sizeof(big_block)] };
			big_block *bb = new (data.get()) big_block( n * sz );
			push( bb, _big_blocks );
			data.release();
			return reinterpret_cast<T *>( bb->data() );
		}

		void *ptr = nullptr;
		bool full = false;
		block *cur = pop( _live );

		if ( cur )
		{
			ptr = cur->alloc( n, full );
			if ( ptr )
			{
				push( cur, full ? _full : _live );
				return reinterpret_cast<T *>( ptr );
			}

			// hrm, couldn't allocate from this block, keep it on
			// the stack and recurse
			return get<T>( n );
		}
		cur = new block;
		ptr = cur->alloc( n, full );
		push( cur, full ? _full : _live );
		return reinterpret_cast<T *>( ptr );
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
		block *cur;
		while ( ( cur = pop( _live ) ) != nullptr )
		{
			++nbasic;
			delete cur;
		}

		while ( ( cur = pop( _full ) ) != nullptr )
		{
			++nbasic;
			delete cur;
		}

		size_t bytes = 0;
		big_block *bb;
		while ( ( bb = pop( _big_blocks ) ) != nullptr )
		{
			bytes += bb->_sz;
			delete bb;
		}

		return std::make_pair( nbasic * chunk_size, bytes );
	}

private:
	struct big_block
	{
		big_block *_next = nullptr;
		size_t _sz = 0;
		big_block( size_t s ) : _next( nullptr ), _sz( s ) {}
		void *data() { uint8_t *raw = reinterpret_cast<uint8_t *>( this ); return raw + sizeof(big_block); }
	};

	struct block
	{
		static constexpr size_t kDataSize = chunk_size - ( sizeof(std::atomic<block *>) + sizeof(std::atomic<size_t>) );

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

		block *_next = nullptr;
		std::atomic<size_t> _offset{ 0 };
		uint8_t _data[kDataSize];
	};
	static_assert( sizeof(block) == chunk_size, "Invalid data size / alignment computation" );

	template <typename B>
	inline B *pop( std::atomic<B *> &head )
	{
		B *old = head.load( std::memory_order_acquire );

		B *repl;
		do {
			if ( old == nullptr )
				return nullptr;

			repl = old->_next;
		} while( ! head.compare_exchange_weak( old, repl,
											   std::memory_order_acquire,
											   std::memory_order_relaxed ) );
		return old;
	}

	template <typename B>
	inline void push( B *p, std::atomic<B *> &head )
	{
		B *old = head.load( std::memory_order_acquire );
		do
		{
			p->_next = old;
		} while ( ! head.compare_exchange_weak( old, p,
												std::memory_order_acquire,
												std::memory_order_relaxed ) );
	}

	std::atomic<block *> _live;
	std::atomic<block *> _full;
	std::atomic<big_block *> _big_blocks;
};

} // namespace scene

