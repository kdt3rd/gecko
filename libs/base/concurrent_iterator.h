// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iterator>

#include "utility.h"

namespace base
{
/// @brief iterator tied to concurrent_vector
///
/// Basically a standard iterator (i.e. std::iterator), except that
/// we are specializing to handle the concurrency case. We stash off
/// the storage that the iterator is iterating over (assuming the
/// container is implemented using create a new list and swap when
/// the container is changed). To make this work more of the time
/// there is a special definition where we keep the offset and
/// re-derive the pointer from the storage when the iterator is
/// actually dereferenced
template <typename Iter, typename Storage> class concurrent_iterator
{
    using _traits_type = std::iterator_traits<Iter>;

public:
    using storage_type    = Storage;
    using iterator_type   = Iter;
    using value_type      = typename _traits_type::value_type;
    using difference_type = typename _traits_type::difference_type;
    using reference       = typename _traits_type::reference;
    using pointer         = typename _traits_type::pointer;

    concurrent_iterator( void ) noexcept = default;

    explicit concurrent_iterator(
        storage_type s, iterator_type p, difference_type offset ) noexcept
        : _storage( std::move( s ) ), _ptr( p ), _offset( offset )
    {}

    concurrent_iterator &operator++( void ) noexcept
    {
        ++_offset;
        return *this;
    }
    concurrent_iterator operator++( int ) noexcept
    {
        return concurrent_iterator( _storage, _ptr, _offset++ );
    }

    concurrent_iterator &operator--( void ) noexcept
    {
        --_offset;
        return *this;
    }
    concurrent_iterator operator--( int ) noexcept
    {
        return concurrent_iterator( _storage, _offset-- );
    }

    bool operator==( const concurrent_iterator &o ) const
    {
        return _storage == o._storage && _offset == o._offset;
    }
    bool operator!=( const concurrent_iterator &o ) const
    {
        return !( *this == o );
    }
    reference operator*(void)const noexcept { return *( _ptr + _offset ); }
    pointer   operator->(void)const noexcept { return ( _ptr + _offset ); }
    reference operator[]( difference_type n ) const noexcept
    {
        return *( _ptr + ( _offset + n ) );
    }

    concurrent_iterator &operator+=( difference_type n ) noexcept
    {
        _offset += n;
        return *this;
    }
    concurrent_iterator operator+( difference_type n ) const noexcept
    {
        return concurrent_iterator( _storage, _ptr, _offset + n );
    }
    concurrent_iterator &operator-=( difference_type n ) noexcept
    {
        _offset -= n;
        return *this;
    }
    concurrent_iterator operator-( difference_type n ) const noexcept
    {
        return concurrent_iterator( _storage, _ptr, _offset - n );
    }

    /////////////////////////
    /// non-standard API for handling the concurrent bits to
    /// make sure that range-based for and things work by
    /// default in more scenarios
    storage_type    storage( void ) const noexcept { return _storage; }
    iterator_type   iter( void ) const noexcept { return _ptr; }
    difference_type base( void ) const noexcept { return _offset; }

private:
    storage_type    _storage = {};
    iterator_type   _ptr     = nullptr;
    difference_type _offset  = 0;
};

template <typename IterA, typename IterB, typename Container>
inline bool operator==(
    const concurrent_iterator<IterA, Container> &a,
    const concurrent_iterator<IterB, Container> &b ) noexcept
{
    return a.storage() == b.storage() && a.base() == b.base();
}

template <typename Iter, typename Container>
inline bool operator==(
    const concurrent_iterator<Iter, Container> &a,
    const concurrent_iterator<Iter, Container> &b ) noexcept
{
    return a.storage() == b.storage() && a.base() == b.base();
}

template <typename IterA, typename IterB, typename Container>
inline bool operator!=(
    const concurrent_iterator<IterA, Container> &a,
    const concurrent_iterator<IterB, Container> &b ) noexcept
{
    return !( a == b );
}

template <typename Iter, typename Container>
inline bool operator!=(
    const concurrent_iterator<Iter, Container> &a,
    const concurrent_iterator<Iter, Container> &b ) noexcept
{
    return !( a == b );
}

template <typename IterA, typename IterB, typename Container>
inline bool operator<(
    const concurrent_iterator<IterA, Container> &a,
    const concurrent_iterator<IterB, Container> &b ) noexcept
{
    // TODO: this isn't correct, so using this in a map will
    // be bad but this is a concurrent thing and the iterator
    // is not guaranteed to be stable anyway
    if ( a.iter() != b.iter() )
        return false;
    return a.base() < b.base();
}

template <typename Iter, typename Container>
inline bool operator<(
    const concurrent_iterator<Iter, Container> &a,
    const concurrent_iterator<Iter, Container> &b ) noexcept
{
    // TODO: this isn't correct, so using this in a map will
    // be bad but this is a concurrent thing and the iterator
    // is not guaranteed to be stable anyway
    if ( a.iter() != b.iter() )
        return false;
    return a.base() < b.base();
}

template <typename Iter, typename Container>
inline concurrent_iterator<Iter, Container> operator+(
    typename concurrent_iterator<Iter, Container>::difference_type a,
    const concurrent_iterator<Iter, Container> &                   b ) noexcept
{
    return concurrent_iterator<Iter, Container>(
        b.storage(), b.iter(), b.base() + a );
}

// TODO: define the other operators?

} // namespace base
