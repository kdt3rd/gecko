// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "contract.h"

#include <atomic>
#include <memory>

////////////////////////////////////////

namespace base
{
/// @brief provides a by-construction power of 2 sized queue
///
/// This enables a one-time allocation of a fixed buffer and
/// when available, uses atomic to provide fast push/pop in a
/// multi-producer / multi-consumer (MPMC) thread safe manner
template <typename Value> class bounded_safe_queue
{
public:
    typedef Value value_type;

    bounded_safe_queue( void ) = default;

    /// @brief allocating constructor
    ///
    /// This allocates a queue that is 2^(log2size) entries
    ///
    /// constructor is by definition not thread safe, ensure that this
    /// is fully constructed before being made available to multiple
    /// threads
    bounded_safe_queue( size_t log2size )
        : _buffer( new storage_t[1 << log2size] )
        , _buffer_index_mask( ( 1 << log2size ) - 1 )
        , _push_pos( 0 )
        , _pop_pos( 0 )
    {
        size_t N = 1 << log2size;
        if ( N < 4 )
            throw_runtime(
                "lock-free bounded queue of size {0} (log2 size {1}) is too small to be effective",
                N,
                log2size );

        storage_t *initBuf = _buffer.get();
        for ( size_t i = 0; i != N; ++i )
            initBuf[i].tag.store( i, std::memory_order_relaxed );
    }
    /// @brief move constructor
    ///
    /// assumes ownership, allowing a default constructed thing to be
    /// re-constructed such that these things can appear in a vector
    /// or something
    ///
    /// NB: NOT THREAD SAFE
    bounded_safe_queue( bounded_safe_queue &&other )
        : _buffer( std::move( other._buffer ) )
        , _buffer_index_mask( other._buffer_index_mask )
        , _push_pos( other._push_pos.load() )
        , _pop_pos( other._pop_pos.load() )
    {}

    /// @brief move assignment
    ///
    /// assumes ownership, allowing a default constructed thing to be
    /// re-constructed such that these things can appear in a vector
    /// or something
    ///
    /// NB: NOT THREAD SAFE
    bounded_safe_queue &operator=( bounded_safe_queue &&other )
    {
        _buffer            = std::move( other._buffer );
        _buffer_index_mask = std::move( other._buffer_index_mask );
        _push_pos.store( other._push_pos.load() );
        _pop_pos.store( other._pop_pos.load() );
        return *this;
    }

    /// @brief returns whether the underlying storage is lock free
    bool is_lock_free( void ) const { return _push_pos.is_lock_free(); }

    /// @brief attempts to push a value on the queue.
    ///
    /// Returns true when successful, false when the queue is full
    bool try_push( const value_type &data )
    {
        storage_t *store;
        size_t     loc = _push_pos.load( std::memory_order_relaxed );
        for ( ;; )
        {
            store = _buffer.get() + ( loc & _buffer_index_mask );
            // load the tag. if another thread has stored here, the
            // value will already be loc + 1. Otherwise if a free has
            // happened and we've looped around, the tag will be the
            // NEXT iteration of the position that will result in this
            // location.
            size_t   curtag = store->tag.load( std::memory_order_acquire );
            intptr_t tagdelta =
                static_cast<intptr_t>( curtag ) - static_cast<intptr_t>( loc );
            if ( tagdelta == 0 )
            {
                if ( _push_pos.compare_exchange_weak(
                         loc, loc + 1, std::memory_order_relaxed ) )
                    break;
            }
            else if ( tagdelta < 0 )
                return false;
            else
                loc = _push_pos.load( std::memory_order_relaxed );
        }
        store->value = data;
        store->tag.store( loc + 1, std::memory_order_release );
        return true;
    }

    /// @brief attempts to push a value on the queue using rvalue semantics
    ///
    /// Returns true when successful, false when the queue is full
    bool try_emplace( value_type &&data )
    {
        storage_t *store;
        size_t     loc = _push_pos.load( std::memory_order_relaxed );
        for ( ;; )
        {
            store           = _buffer.get() + ( loc & _buffer_index_mask );
            size_t   curtag = store->tag.load( std::memory_order_acquire );
            intptr_t tagdelta =
                static_cast<intptr_t>( curtag ) - static_cast<intptr_t>( loc );
            if ( tagdelta == 0 )
            {
                if ( _push_pos.compare_exchange_weak(
                         loc, loc + 1, std::memory_order_relaxed ) )
                    break;
            }
            else if ( tagdelta < 0 )
                return false;
            else
                loc = _push_pos.load( std::memory_order_relaxed );
        }
        store->value = std::move( data );
        store->tag.store( loc + 1, std::memory_order_release );
        return true;
    }

    /// @brief attempts to pop a value from the queue
    ///
    /// Returns true when successful, false when the queue is empty
    bool try_pop( value_type &v )
    {
        storage_t *to_pull;
        size_t     loc = _pop_pos.load( std::memory_order_relaxed );
        for ( ;; )
        {
            to_pull           = _buffer.get() + ( loc & _buffer_index_mask );
            size_t   curtag   = to_pull->tag.load( std::memory_order_acquire );
            intptr_t tagdelta = static_cast<intptr_t>( curtag ) -
                                static_cast<intptr_t>( loc + 1 );
            if ( tagdelta == 0 )
            {
                // try to update to pull from the next queue entry in the ring
                if ( _pop_pos.compare_exchange_weak(
                         loc, loc + 1, std::memory_order_relaxed ) )
                    break;
            }
            else if ( tagdelta < 0 )
                return false;
            else
            {
                loc = _pop_pos.load( std::memory_order_relaxed );
            }
        }

        // we've got our location, extract the value
        v = std::move( to_pull->value );

        // store off the next value the _push_pos will be when we go
        // to store at this location.
        //
        // we could store _buffer_size in the object, but that's
        // _buffer_index_mask + 1, so save a register load from this
        // and just addd 1 to the mask
        to_pull->tag.store(
            loc + _buffer_index_mask + 1, std::memory_order_release );

        return true;
    }

private:
    bounded_safe_queue( const bounded_safe_queue & ) = delete;
    bounded_safe_queue &operator=( const bounded_safe_queue & ) = delete;
#if __cplusplus > 201402L
    inline constexpr size_t alignment_offset =
        std::hardware_destructive_interference_size;
#else
    static constexpr size_t alignment_offset = 64;
#endif
    struct storage_t
    {
        std::atomic<size_t> tag;
        value_type          value;
    };

    std::unique_ptr<storage_t> _buffer;
    const size_t               _buffer_index_mask = 0;

    // avoid cache line mixing across cores
    alignas( alignment_offset ) std::atomic<size_t> _push_pos;
    alignas( alignment_offset ) std::atomic<size_t> _pop_pos;
};

} // namespace base
