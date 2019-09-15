// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "contract.h"

#include <atomic>
#include <cstddef>
#include <cstdint>

////////////////////////////////////////

namespace base
{
template <typename T> class lock_free_list_node
{
public:
    lock_free_list_node( void ) : _lf_list_next( nullptr ) {}

    inline T *next( void ) const
    {
        return _lf_list_next.load( std::memory_order_relaxed );
    }
    std::atomic<T *> _lf_list_next;
};

/// @brief keeps a simple single linked list of items
///
/// values are not owned by the list in any way
template <typename T> class lock_free_list
{
public:
    using value_type = T;
    static_assert(
        std::is_base_of<lock_free_list_node<value_type>, value_type>::value,
        "list value must be derived from lock_free_list_node using CRTP" );

    lock_free_list( void ) : _head( lf_head() ) {}

    inline value_type *steal( void )
    {
        lf_head cur = _head.load( std::memory_order_acquire );
        lf_head head;
        do
        {
            head.ptr = nullptr;
            head.tag = cur.tag + 1;
            if ( !_head.compare_exchange_weak(
                     cur,
                     head,
                     std::memory_order_release,
                     std::memory_order_acquire ) )
                continue;
        } while ( false );

        return cur.ptr;
    }

    inline void push( value_type *v )
    {
        lf_head cur  = _head.load( std::memory_order_relaxed );
        lf_head head = { v };
        do
        {
            head.tag = cur.tag + 1;
            v->_lf_list_next.store( cur.ptr, std::memory_order_relaxed );
        } while ( !_head.compare_exchange_weak(
            cur, head, std::memory_order_release, std::memory_order_relaxed ) );
    }

    inline value_type *try_pop( void )
    {
        lf_head cur = _head.load( std::memory_order_acquire );
        lf_head head;
        while ( cur.ptr != nullptr )
        {
            head.ptr = cur.ptr->_lf_list_next.load( std::memory_order_relaxed );
            head.tag = cur.tag + 1;
            if ( _head.compare_exchange_weak(
                     cur,
                     head,
                     std::memory_order_release,
                     std::memory_order_acquire ) )
                break;
        }
        return cur.ptr;
    }

    inline void clear( void )
    {
        lf_head head;
        _head.store( head );
    }

    inline value_type *unsafe_front( void ) const
    {
        return _head.load( std::memory_order_relaxed ).ptr;
    }

private:
    struct lf_head
    {
        value_type *   ptr = nullptr;
        std::uintptr_t tag = 0;
    };
    static_assert(
        sizeof( lf_head ) == ( 2 * sizeof( void * ) ),
        "invalid size for head pointer" );
    alignas( 2 * sizeof( void * ) ) std::atomic<lf_head> _head;
};

// what about a double linked list
//
// this can easily suffer from either the classic ABA problem or not a
// large enough atomic operation to effect the change.
//
// given a list
//
// a -> b -> c
//
// rcu (read copy update) removes b by updating a to point to c
//
// a      -> c
//         ^
//        /
//       b
//
// while leaving b continuing to point to c. Then the actor performing
// the remove waits for quiescent state, at which point it knows no
// other threads are accessing b and can then free b
//
// can we use this idea to implement a double linked list?
//
//   ->   ->
// a    b    c
//   <-   <-
//
// to remove b, we can't really update all the pointers at once to
// make it atomic. so what do we do:
//
// if we assume someone is either traversing the list forward (and can
// constrain it so w/ appropriate iterators), then we can reduce the
// problem to ensuring that the access patterns are consistent, so we
// reduce to having to update two singly linked lists.
//
// Further, if we can say something like RCU that the object will not
// be freed or reclaimed until the state of that object is quiesced
// (meaning objects that are potentially reading or accessing it), we
// can avoid the aba issue entirely.
//
// so if we have iterators to access the list, and do not provide any
// other access methods, we can encapsulate this and provide
// consistent forward and backward iteration, just not both at the
// same time (consistently)
//
//
} // namespace base
