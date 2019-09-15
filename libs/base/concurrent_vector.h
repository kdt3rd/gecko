// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "concurrent_iterator.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <limits>

namespace base
{
/// @brief keeps a concurrent vector of T
///
/// This has a few semantic differences to std::vector, but a useful
/// subset of the API, and more is possible as needed.
///
/// Changes to the size of the vector may represent a re-allocation.
/// This is done in a semi-safe way in that it is done in a manner
/// inspired by the quiescent state based reclamation (QSBR) model:
///  - all iterators keep a reference to the data, such that when
///    all iterators go out of scope and are destroyed, the old
///    memory will be deleted
///  - an iterator may be pointing at old data, but will at least
///    have a consistent view of the data
///
/// The interested reader will notice almost immediately that the fast,
/// random access usually available in vector (i.e. operator[]) is not
/// currently available for concurrent_vector. Additionally, we do NOT
/// provide the convenience routines such as front, back, and data / cdata.
/// These are of course trivial, but the object lifetimes don't allow us
/// to hold on to the backing store of the object, so another thread
/// could be destroying the reference just retrieved from the call.
///
template <typename T> class concurrent_vector
{
public:
    using value_type = T;
    static_assert(
        std::is_pod<value_type>::value, "only support pod types currently" );
    using reference       = T &;
    using const_reference = const T &;
    using pointer         = T *;
    using const_pointer   = const T *;

    using size_type       = size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = base::concurrent_iterator<pointer, std::shared_ptr<void>>;
    using const_iterator =
        base::concurrent_iterator<const_pointer, std::shared_ptr<void>>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    concurrent_vector( void ) = default;
    ~concurrent_vector( void ) = default;
    concurrent_vector( const concurrent_vector & ) = delete;
    concurrent_vector &operator=( const concurrent_vector & ) = delete;
    concurrent_vector( concurrent_vector &&o ) noexcept
    {
        auto oldl =
            std::atomic_load_explicit( &o._data, std::memory_order_relaxed );
        do
        {
            storage_type newl;
            if ( std::atomic_compare_exchange_weak_explicit(
                     &o._data,
                     &oldl,
                     newl,
                     std::memory_order_release,
                     std::memory_order_relaxed ) )
                break;
        } while ( true );
        atomic_store( &_data, oldl );
    }
    concurrent_vector &operator=( concurrent_vector &&o ) noexcept
    {
        if ( this != &o )
        {
            concurrent_vector tmp( std::move( o ) );
            swap( tmp );
        }
        return *this;
    }

    /// NB: remember that this is only a transient state unless you
    /// know more about threads
    bool empty( void ) const
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        return !static_cast<bool>( p );
    }

    /// NB: remember that the size of the list may change between when
    /// you call this and actually try to use it (unless you know otherwise)
    size_type size( void ) const
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        return ( p ) ? p->size.load( std::memory_order_relaxed ) : 0;
    }

    size_type capacity( void ) const
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        return ( p ) ? p->capacity : 0;
    }

    size_type max_size( void ) const noexcept
    {
        return std::numeric_limits<difference_type>::max() /
               sizeof( value_type );
    }

    /// @brief clear the contents of the vector.
    /// Unlike a std::vector, this will actually free the memory
    /// associated with the vector
    void clear( void )
    {
        // TODO: Should we instead just shrink the size to 0 and
        // reset the values?
        std::atomic_store( &_data, storage_type() );
    }

    void shrink_to_fit( void )
    {
        // this is non-binding per the stl, but should do our best as needed
    }

    void reserve( size_type n ) const
    {
        auto oldl =
            std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        do
        {
            if ( oldl && oldl->capacity >= n )
                return;
            storage_type newl = make_new_list( n, oldl );
            if ( std::atomic_compare_exchange_weak_explicit(
                     &_data,
                     &oldl,
                     newl,
                     std::memory_order_release,
                     std::memory_order_relaxed ) )
                break;
        } while ( true );
    }

    /// @brief standard begin / end iteration
    /// One diversion from a standard list is that the "end" iterator
    /// is currently a special tag, and the iterators keep a copy of the
    /// list. This allows the list to change between a begin and end
    /// call, yet the begin / end will still present a valid list
    /// (although now outdated)
    /// The HUGE issue with this is that "end" is a special value, yet
    /// being returned as a normal iterator, which means that code
    /// could apply the normal operators on it and make it totally bogus
    /// and we'd have no idea
    iterator begin( void ) noexcept
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        return iterator( std::move( p ), p ? p->values : nullptr, 0 );
    }
    iterator end( void ) noexcept
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        if ( p )
            return iterator(
                std::move( p ),
                p->values,
                static_cast<typename iterator::difference_type>(
                    p->size.load( std::memory_order_relaxed ) ) );
        return iterator( std::move( p ), nullptr, 0 );
    }
    const_iterator begin( void ) const noexcept
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        return const_iterator( std::move( p ), p ? p->values : nullptr, 0 );
    }
    const_iterator end( void ) const noexcept
    {
        auto p = std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        if ( p )
            return const_iterator(
                std::move( p ),
                p->values,
                static_cast<typename const_iterator::difference_type>(
                    p->size.load( std::memory_order_relaxed ) ) );
        return const_iterator( std::move( p ), nullptr, 0 );
    }

    /// TODO: we can do these, however to do the same sort of ~consistent
    /// access as the forward iterator, we will have to implement a custom
    /// reverse iterator
    reverse_iterator rbegin( void ) noexcept
    {
        return reverse_iterator( end() );
    }
    reverse_iterator rend( void ) noexcept
    {
        return reverse_iterator( begin() );
    }
    const_reverse_iterator rbegin( void ) const noexcept
    {
        return const_reverse_iterator( end() );
    }
    const_reverse_iterator rend( void ) const noexcept
    {
        return const_reverse_iterator( begin() );
    }

    const_iterator         cbegin( void ) const noexcept { return begin(); }
    const_iterator         cend( void ) const noexcept { return end(); }
    const_reverse_iterator crbegin( void ) const noexcept { return rbegin(); }
    const_reverse_iterator crend( void ) const noexcept { return rend(); }

    void push_back( const value_type &x )
    {
        return emplace_back( value_type( x ) );
    }
    void push_back( value_type &&x ) { emplace_back( std::move( x ) ); }

    /// @brief constructs an item at the end of the list
    ///
    /// NB: This currently has a partial race condition with multiple
    /// writers (see the comments for deeper discussion)
    /// TODO: In c++17, this returns a reference?
    template <typename... Args> void emplace_back( Args &&... a )
    {
        auto oldl =
            std::atomic_load_explicit( &_data, std::memory_order_relaxed );
        do
        {
            size_t newsize = 1;
            if ( oldl )
            {
                // TODO: not entirely safe from a multiple writer
                //       multiple reader scenario
                // consider:
                // capacity has plenty of room
                // thread A increments and gets an index X
                // thread B increments and gets an index Y
                // thread B stores value at index Y
                // thread B increments the size
                // thread C is iterating, gets to the index for A
                //   - this looks like a valid index due to B storing size
                //   - accesses uninitialized value not yet stored by A
                //   - semi ok, but also misses value from B
                //
                // if capacity is always only one more than current size
                // this can be safe and still cuts the number of re-allocs
                // in half
                //
                // We could fix this be enforcing there's always at most
                // one writer - basically a writer only lock...
                size_t idx =
                    oldl->nextidx.fetch_add( 1, std::memory_order_relaxed );
                if ( idx < oldl->capacity )
                {
                    oldl->values[idx] =
                        value_type( std::forward<Args>( a )... );
                    oldl->size.fetch_add( 1, std::memory_order_release );
                    return;
                }
                newsize += oldl->size.load( std::memory_order_relaxed );
            }

            storage_type newl = make_new_list( newsize, oldl );
            newl->values[newsize - 1] =
                value_type( std::forward<Args>( a )... );
            //new ( newl->values + ( newsize - 1 ) )
            //    value_type( std::forward<Args>( a )... );

            // if we succeed we're done, otherwise someone else changed
            // something first, so loop and re-try
            if ( std::atomic_compare_exchange_weak_explicit(
                     &_data,
                     &oldl,
                     newl,
                     std::memory_order_release,
                     std::memory_order_relaxed ) )
                break;
        } while ( true );
    }

    // todo: implement insert, erase, append
    //iterator insert( iterator pos, const value_type &v );
    //iterator insert( const_iterator pos, const value_type &v );
    //iterator insert( const_iterator pos, value_type &&v );
    //iterator insert( const_iterator pos, size_type count, const T &value );
    //template <typename InputIter>
    //iterator insert( iterator pos, InputIter first, InputIter last );
    //template <typename InputIter>
    //iterator insert( const_iterator pos, InputIter first, InputIter last );
    //iterator
    //insert( const_iterator pos, std::initializer_list<value_type> ilist );
    //template <typename... Args>
    //iterator emplace( const_iterator pos, Args&&... args );
    //iterator erase( iterator pos );
    //iterator erase( const_iterator pos );
    //iterator erase( iterator first, iterator last );
    //iterator erase( const_iterator first, const_iterator last );
    //void pop_back( void );
    //void resize( size_type count );
    //void resize( size_type count, const value_type &v );

    /// NB: if there are multiple threads accessing this, you may not
    /// get what you expect, in that this isn't an atomic operations,
    /// but instead 2 operations (although should never leak memory)
    ///
    /// in c++17 this gains some noexcept guarantees based on allocator
    void swap( concurrent_vector &other )
    {
        atomic_store( &_data, atomic_exchange( &other._data, _data ) );
    }

private:
    // TODO: we could consider doing a bit more with the idea of
    // allocating extra storage and having an (atomic) size variable
    // and then a capacity. This would handle push_back / pop_back
    // far more efficiently.
    // This would also make the pattern of reserve and push_back
    // more efficient, assuming that where these lists will be used
    // it needs to be concurrent but isn't likely to have much
    // overlap
    // NB: until then, we will default initialize the allocations
    // so at least implementations will get a nullptr or 0
    //
    // erases from the middle will still have to be a full list swap
    //
    // TBD: unclear that locking the cpu bus everytime we want to pull
    // the size is a good idea. perhaps there is another way?
    struct CurrentList
    {
        std::atomic<size_type> size;
        size_type              capacity;
        std::atomic<size_type> nextidx;
        // TODO: make this uninitialized space such that
        // we can run the appropriate constructors and support non pod types
        value_type values[1];
    };
    using storage_type = std::shared_ptr<CurrentList>;

    template <typename Value> struct Allocator
    {
        typedef Value value_type;
        Allocator() = default;
        Allocator( size_t nextra ) noexcept : _extra( nextra ) {}
        template <typename X>
        constexpr Allocator( const Allocator<X> &o ) noexcept
            : _extra( o._extra )
        {}

        value_type *allocate( std::size_t n )
        {
            if ( n > std::size_t( -1 ) / sizeof( T ) )
                throw std::bad_alloc();

            size_t bytes = n * ( sizeof( value_type ) + _extra );
            void * ptr   = std::malloc( bytes );
            if ( ptr )
                return static_cast<value_type *>( ptr );
            throw std::bad_alloc();
        }
        void   deallocate( value_type *p, std::size_t ) { std::free( p ); }
        size_t _extra = 0;
    };

    inline storage_type make_new_list( size_t count, const storage_type &oldl )
    {
        using allocator = Allocator<CurrentList>;
        size_t capacity;
        size_t cursize = 0;
        if ( oldl )
            cursize = oldl->size.load( std::memory_order_relaxed );
        capacity          = std::max( cursize * 2, count );
        size_t extrabytes = capacity * sizeof( value_type );
        auto ret = std::allocate_shared<CurrentList>( allocator( extrabytes ) );
        ret->size    = count;
        ret->nextidx = count;
        // we always have 1 extra due to the structure definition
        ret->capacity  = capacity + 1;
        size_t defInit = 0;
        if ( oldl )
        {
            for ( size_t N = std::min( count, cursize ); defInit != N;
                  ++defInit )
            {
                //new ( ret->values + i ) value_type( oldl->values[i] );
                ret->values[defInit] = oldl->values[defInit];
            }
        }
        for ( ; defInit != capacity + 1; ++defInit )
            ret->values[defInit] = value_type();
        return ret;
    }

    storage_type _data;
};

} // namespace base

namespace std
{
template <typename T>
inline void swap( base::concurrent_vector<T> &a, base::concurrent_vector<T> &b )
{
    a.swap( b );
}
}
