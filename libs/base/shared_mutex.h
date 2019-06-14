// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "semaphore.h"
#include "utility.h"

#include <mutex>
#include <stdexcept>
#include <system_error>

////////////////////////////////////////

namespace base
{
///
/// @brief Class shared_mutex provides an API compatible version of a
/// std::shared_mutex that doesn't become available until C++17
///
/// Uses semaphore method based on (extended w/ try semantics):
/// http://preshing.com/20150316/semaphores-are-surprisingly-versatile/
class shared_mutex
{
private:
    inline shared_mutex( const shared_mutex & ) = delete;
    inline shared_mutex( shared_mutex && )      = delete;
    inline shared_mutex &operator=( const shared_mutex & ) = delete;
    inline shared_mutex &operator=( shared_mutex && ) = delete;

public:
    using native_handle_type = void *;

    shared_mutex( void );
    ~shared_mutex( void );

    void lock( void );
    bool try_lock( void );
    void unlock( void );

    /// allows multiple readers
    void lock_shared( void );
    bool try_lock_shared( void );
    void unlock_shared( void );

    inline native_handle_type native_handle( void ) { return this; }

private:
    std::atomic<uint64_t> _state;
    semaphore             _read_sem;
    semaphore             _write_sem;
};

/// @brief similar to std::lock_guard, but for shared_mutex read locks
template <typename M> class shared_lock_guard
{
public:
    using mutex_type = M;
    explicit shared_lock_guard( mutex_type &m ) : _m( m ) { _m.lock_shared(); }
    shared_lock_guard( mutex_type &m, std::adopt_lock_t ) : _m( m ) {}
    ~shared_lock_guard( void ) { _m.unlock_shared(); }

    shared_lock_guard( const shared_lock_guard & ) = delete;
    shared_lock_guard &operator=( const shared_lock_guard & ) = delete;
    shared_lock_guard( shared_lock_guard && )                 = delete;
    shared_lock_guard &operator=( shared_lock_guard && ) = delete;

private:
    mutex_type &_m;
};

/// @brief similar to shared_mutex, provides a (mostly) compatible
/// implementation of shared_lock for c++11
///
/// NB: Currently, no attempt to replicate the timeout variants of
/// locking has been made due to lack of current need.
template <typename M> class shared_lock
{
public:
    using mutex_type = M;

    inline shared_lock( void ) noexcept : _ptr( nullptr ), _owns( false ) {}
    inline shared_lock( shared_lock &&o ) noexcept : shared_lock()
    {
        swap( o );
    }

    explicit inline shared_lock( mutex_type &m ) : _ptr( &m ), _owns( true )
    {
        m.lock_shared();
    }

    shared_lock( mutex_type &m, std::defer_lock_t ) noexcept
        : _ptr( &m ), _owns( false )
    {}
    shared_lock( mutex_type &m, std::try_to_lock_t )
        : _ptr( &m ), _owns( m.try_lock_shared() )
    {}
    shared_lock( mutex_type &m, std::adopt_lock_t ) : _ptr( &m ), _owns( true )
    {}

    shared_lock( const shared_lock & ) = delete;
    shared_lock &operator=( const shared_lock & ) = delete;

    shared_lock &operator=( shared_lock &&o ) noexcept
    {
        shared_lock( std::move( o ) ).swap( *this );
        return *this;
    }
    inline ~shared_lock( void )
    {
        if ( _owns )
            _ptr->unlock_shared();
    }

    void lock( void )
    {
        check_lockable();
        _ptr->lock_shared();
        _owns = true;
    }

    void try_lock( void )
    {
        check_lockable();
        return _owns = _ptr->try_lock_shared();
    }

    void unlock( void )
    {
        if ( !_owns )
            throw std::system_error( std::make_error_code(
                std::errc::resource_deadlock_would_occur ) );

        _ptr->unlock_shared();
        _owns = false;
    }

    void swap( shared_lock &o ) noexcept
    {
        std::swap( _ptr, o._ptr );
        std::swap( _owns, o._owns );
    }

    mutex_type *release( void ) noexcept
    {
        _owns = false;
        return base::exchange( _ptr, nullptr );
    }

    inline bool owns_lock( void ) const noexcept { return _owns; }
    explicit    operator bool( void ) const noexcept { return _owns; }
    mutex_type *mutex( void ) const noexcept { return _ptr; }

private:
    inline void check_lockable( void )
    {
        if ( _ptr == nullptr )
            throw std::system_error(
                std::make_error_code( std::errc::operation_not_permitted ) );
        if ( _owns )
            throw std::system_error( std::make_error_code(
                std::errc::resource_deadlock_would_occur ) );
    }

    mutex_type *_ptr;
    bool        _owns;
};

template <typename M>
inline void swap( shared_lock<M> &a, shared_lock<M> &b ) noexcept
{
    a.swap( b );
}

} // namespace base

namespace std
{
template <typename M>
inline void swap( base::shared_lock<M> &a, base::shared_lock<M> &b ) noexcept
{
    base::swap( a, b );
}

} // namespace std
