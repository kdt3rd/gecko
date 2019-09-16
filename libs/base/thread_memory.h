// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#ifdef _WIN32
#    define NOMINMAX
#    include <windows.h>
#    undef NOMINMAX
#else
#    include <pthread.h>
#endif
#include "contract.h"
#include "utility.h"

////////////////////////////////////////

namespace base
{
template <typename T> class thread_specific_default_ctor
{
public:
    static_assert(
        std::is_default_constructible<T>::value,
        "Thread-local value must be default constructible, switch to thread_specific_null_ctor or provide your own" );
    constexpr thread_specific_default_ctor( void ) noexcept = default;

    T *operator()( void ) const { return new T; }
};

template <typename T> class thread_specific_null_ctor
{
public:
    constexpr thread_specific_null_ctor( void ) noexcept = default;

    T *operator()( void ) const { return nullptr; }
};

///
/// @brief Class thread_ptr a provides a thread-specific pointer
///
/// This pattern is commonly used to dynamically create
/// thread-specific data, and has different semantics than a thread
/// local variable which has static semantics - there can be multiple
/// of these objects.
///
/// The lifetime of thread_ptr should be longer than the lifetime of
/// any threads using the pointer to ensure memory is properly
/// reclaimed. As a special exception, the calling thread that causes
/// this pointer to be created can contribute in the use of the thread
/// pointer as long as it is also the thread that ends up causing this
/// pointer object to be deleted.
///
/// The overall interface of thread_ptr has similar behavior to
/// unique_ptr, besides the obvious per-thread differences. It also
/// does not include the specialization for arrays, so caveat emptor.
///
/// Additionally, a constructor object is provided to enable custom
/// instantiation of the pointed to object. A constructor @sa
/// thread_specific_default_ctor version is provided as a
/// default. When a thread_ptr is accessed by a thread, if the value
/// is null, the constructor is called. if your application needs
/// specific tests, then use the also provided null ctor @sa
/// thread_specific_null_ctor.
///
template <
    typename T,
    typename D = std::default_delete<T>,
    typename C = thread_specific_default_ctor<T>>
class thread_ptr
{
    // TODO: check the deleter constraints and all the conversion things...
    // TODO: enable lvalue reference type for deleter? Right now we're duplicating
    //       the main one to a reference in each thread's holder
    struct thread_ptr_holder
    {
        using pointer = T *;
        using deleter = D;
        thread_ptr_holder( const deleter &d ) noexcept
            : _ptr( nullptr ), _del( d )
        {}
        thread_ptr_holder( pointer p, const deleter &d ) noexcept
            : _ptr( p ), _del( d )
        {}
        ~thread_ptr_holder( void ) { _del( _ptr ); }

        pointer release( void ) noexcept
        {
            pointer p = nullptr;
            std::swap( _ptr, p );
            return p;
        }

        void reset( pointer p ) noexcept
        {
            pointer tmp = _ptr;
            _ptr        = p;
            _del( tmp );
        }

        pointer get( void ) { return _ptr; }

        T *      _ptr;
        const D &_del;
    };

public:
    using pointer      = typename thread_ptr_holder::pointer;
    using element_type = T;
    using deleter_type = D;
    using ctor_type    = C;

    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    thread_ptr( void ) { init( nullptr ); }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    thread_ptr( std::nullptr_t ) { init( nullptr ); }

    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    explicit thread_ptr( pointer p ) { init( p ); }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( pointer p, const deleter_type &d ) : _del( d )
    {
        init( p );
    }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( pointer p, deleter_type &&d ) : _del( std::move( d ) )
    {
        init( p );
    }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( pointer p, const ctor_type &c ) : _cons( c )
    {
        init( p );
    }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( pointer p, ctor_type &&c ) : _cons( std::move( c ) )
    {
        init( p );
    }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( pointer p, const deleter_type &d, const ctor_type &c )
        : _del( d ), _cons( c )
    {
        init( p );
    }
    /// NB: NOT noexcept here since we may not be able to create the TLS chunk
    inline thread_ptr( thread_ptr &&o ) noexcept
        : _tls_key{ base::exchange( o._tls_key, kInvalidKey ) }
        , _del{ base::exchange( o._del, deleter_type() ) }
        , _cons{ base::exchange( o._cons, ctor_type() ) }
    {}
    inline ~thread_ptr( void ) { shutdown(); }

    // not directly copyable
    thread_ptr( const thread_ptr & ) = delete;
    thread_ptr &operator=( const thread_ptr & ) = delete;
    thread_ptr &operator                        =( thread_ptr &&o ) noexcept
    {
        thread_ptr( o ).swap( *this );
        return *this;
    }

    // assignable to nullptr...
    inline thread_ptr &operator=( std::nullptr_t ) noexcept
    {
        reset();
        return *this;
    }

    typename std::add_lvalue_reference<element_type>::type inline
    operator*(void)const
    {
        return *get();
    }

    inline pointer operator->(void)const noexcept { return get(); }
    inline pointer get( void ) const noexcept { return thread_holder().get(); }

    inline deleter_type &get_deleter( void ) noexcept { return _del; }
    inline ctor_type &   get_ctor( void ) noexcept { return _cons; }

    explicit inline operator bool( void ) const noexcept
    {
        return get() == pointer() ? false : true;
    }

    inline pointer release( void ) const noexcept
    {
        return thread_holder().release();
    }
    inline void reset( pointer p = pointer() ) const noexcept
    {
        thread_holder().reset( p );
    }

    inline void swap( thread_ptr &o ) noexcept
    {
        std::swap( _tls_key, o._tls_key );
        std::swap( _del, o._del );
        std::swap( _cons, o._cons );
    }

private:
    void init( pointer p )
    {
#ifdef _WIN32
        _tls_key = TlsAlloc();
        if ( _tls_key == TLS_OUT_OF_INDEXES )
            throw_lasterror(
                "Unable to create thread local storage - out of indices" );
        // Need to somehow hook into DllEntryPoint and similar windows api to correctly
        // hook DLL_THREAD_DETACH to clean up memory as a thread goes away
        throw_runtime( "need memory cleanup routine" );
#else
        int r = pthread_key_create( &_tls_key, &thread_ptr::destructor );
        if ( r != 0 )
            throw_location( std::system_error(
                r,
                std::generic_category(),
                "Unable to create thread local storage" ) );
#endif

        if ( p )
            thread_holder().reset( p );
    }

    void shutdown( void )
    {
        if ( _tls_key == kInvalidKey )
            return;

#ifdef _WIN32
        LPVOID v = TlsGetValue( _tls_key );
        // exceptions not allowed in get semantics, so don't check GetLastError
#else
        void *v = pthread_getspecific( _tls_key );
#endif
        destructor( v );

#ifdef _WIN32
        TlsFree( _tls_key );
#else
        pthread_key_delete( _tls_key );
#endif
    }

    inline thread_ptr_holder &thread_holder( void ) const noexcept
    {
#ifdef _WIN32
        LPVOID v = TlsGetValue( _tls_key );
        // exceptions not allowed in get semantics, so don't check GetLastError
#else
        void *v = pthread_getspecific( _tls_key );
#endif
        thread_ptr_holder *th = static_cast<thread_ptr_holder *>( v );
        if ( !th )
        {
            th = new thread_ptr_holder( _cons(), _del );
#ifdef _WIN32
            TlsSetValue( _tls_key, th );
#else
            pthread_setspecific( _tls_key, th );
#endif
        }
        return *th;
    }

    static void destructor( void *p )
    {
        if ( p == nullptr )
            return;

        thread_ptr_holder *th = static_cast<thread_ptr_holder *>( p );
        delete th;
    }

#ifdef _WIN32
    static const DWORD kInvalidKey = TLS_OUT_OF_INDEXES;
    DWORD              _tls_key    = kInvalidKey;
#else
    static const pthread_key_t kInvalidKey = pthread_key_t( -1 );
    pthread_key_t _tls_key = kInvalidKey;
#endif
    deleter_type _del;
    ctor_type    _cons;
};

} // namespace base

namespace std
{
template <typename T, typename D, typename C>
void swap( base::thread_ptr<T, D, C> &a, base::thread_ptr<T, D, C> &b )
{
    a.swap( b );
}

} // namespace std
