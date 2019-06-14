// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "contract.h"

#include <array>
#include <list>
#include <vector>

namespace base
{
////////////////////////////////////////

/// @brief Memory pool allocating in blocks.
template <typename T, size_t Blocks> class memory_pool
{
public:
    /// @brief Value type.
    typedef T value_type;

    /// @brief Pointer type.
    typedef T *pointer;

    /// @brief Const pointer type.
    typedef const T *const_pointer;

    /// @brief Reference type.
    typedef T &reference;

    /// @brief Const reference type.
    typedef const T &const_reference;

    /// @brief Size type.
    typedef std::size_t size_type;

    /// @brief Difference type.
    typedef std::ptrdiff_t difference_type;

    /// @brief Rebind to a different type.
    template <typename U> struct rebind
    {
        typedef memory_pool<U, Blocks> other;
    };

    /// @brief Get address of a reference.
    pointer address( reference x ) const { return &x; }

    /// @brief Get address of a reference.
    const_pointer address( const_reference x ) const { return &x; }

    /// @brief Allocate an object from the pool.
    pointer allocate( size_type n = 1, const_pointer /*hint*/ = nullptr )
    {
        precondition(
            n == 1, "memory pool can only allocate 1 item at a time" );

        if ( _freelist.empty() )
            allocate_block();
        logic_check( !_freelist.empty(), "freelist should not be empty" );

        pointer result = _freelist.back();
        _freelist.pop_back();
        return result;
    }

    /// @brief Deallocate an object back to the pool.
    void deallocate( pointer p, size_type n = 1 )
    {
        precondition(
            n == 1, "memory pool can only deallocate 1 item at a time" );
        _freelist.push_back( p );
    }

    /// @brief Maximum number of objects to allocate.
    size_type max_size( void ) const { return 1; }

    /// @brief Construct an object in-place.
    template <typename U, typename... Args>
    void construct( U *p, Args &&... args )
    {
        ::new ( static_cast<void *>( p ) ) U( std::forward<Args>( args )... );
    }

    /// @brief Call destructor of object.
    template <typename U> void destroy( U *p ) { p->~U(); }

    /// @brief Number of objects available to allocate.
    size_t available( void ) const { return _freelist.size(); }

    /// @brief Capacity of memory pool in bytes.
    size_t capacity( void ) const { return _blocks.size() * Blocks; }

    void clear( void )
    {
        _freelist.clear();
        _blocks.clear();
    }

private:
    // Allocate a block and add it to the free list
    void allocate_block( void )
    {
        _blocks.emplace_back();
        for ( size_t i = 0; i < Blocks; ++i )
            _freelist.push_back( &( _blocks.back()[i] ) );
    }

    std::list<std::array<T, Blocks>> _blocks;
    std::vector<pointer>             _freelist;
};

////////////////////////////////////////

} // namespace base
