// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <functional>
#include <type_traits>

#ifndef _WIN32
#    include <sys/mman.h>
#    include <unistd.h>
#else
#    include <windows.h>
#endif

#include "contract.h"
#include "utility.h"

////////////////////////////////////////

namespace base
{
///
/// @brief Class read_memory_map provides an abstraction around a
/// read-only memory mapped region of a file.
///
/// This roughly mimics unique_ptr, with the exception that it does
/// not allow one to release the pointer.
///
/// One significant difference from unique_ptr is that it assumes the
/// map is for multiple objects of type T, so always provides
/// operator[], compared to unique_ptr template specializing to
/// achieve this behavior.
///
/// @TODO consider adding read / write or just write
/// semantics. Although that requires care, as the semantics can
/// easily become non-standard between different O.S.
///
/// @TODO if the usage of this becomes prevalent, consider an API
/// change to support more functionality, like only releasing portions
/// of the mapping, or perhaps enabling more shared_ptr behavior. Or
/// even just switch to shared_ptr completely with a custom deleter
/// (although that involves malloc/free as you copy the pointer around
/// so may not be usable in all scenarios).
///
template <typename T = void> class read_memory_map
{
public:
    using pointer   = const T *;
    using reference = const T &;

    // TODO: win32 support
#ifdef _WIN32
    using file_handle = HANDLE;
#else
    using file_handle = int;
#endif

    /// @brief default ctor for delayed mapping
    read_memory_map( void ) = default;

    /// @brief construct mapping from a file handle
    read_memory_map( file_handle fd, off_t offset, size_t size )
    {
        reset( fd, offset, size );
    }
    /// @brief destroy the object (unmaps the memory)
    ~read_memory_map( void ) { reset(); }

    /// @brief no copy semantics
    read_memory_map( const read_memory_map & ) = delete;
    /// @brief no copy semantics
    read_memory_map &operator=( const read_memory_map & ) = delete;
    /// @brief move semantics
    read_memory_map( read_memory_map &&o ) noexcept
        : _ptr( base::exchange( o._ptr, nullptr ) )
        , _extra_at_beginning( base::exchange( o._extra_at_beginning, 0 ) )
        , _sz( base::exchange( o._sz, 0 ) )
    {}

    read_memory_map &operator=( read_memory_map &&o ) noexcept
    {
        std::swap( _ptr, o._ptr );
        std::swap( _extra_at_beginning, o._extra_at_beginning );
        std::swap( _sz, o._sz );
        return *this;
    }
    read_memory_map &operator=( std::nullptr_t ) noexcept
    {
        reset();
        return *this;
    }

    /// @brief unmap the resource and free the memory associated
    void reset()
    {
        if ( _ptr )
        {
            char *tmp =
                const_cast<char *>( reinterpret_cast<const char *>( _ptr ) );
            tmp -= _extra_at_beginning;
            size_t unmapsz      = _sz;
            _ptr                = nullptr;
            _extra_at_beginning = 0;
            _sz                 = 0;
#ifndef _WIN32
            int r = munmap( tmp, unmapsz );
            if ( r == -1 )
                throw_errno( "Unable to unmap memory map" );
#else
            UnmapViewOfFile( _ptr );
#endif
        }
    }

    /// @brief maps the specified offset and size of the file.
    ///
    /// This may actually reserve more, up to the page alignment required for the O.S.
    void reset( file_handle fd, off_t offset, size_t size )
    {
        reset();
#ifndef _WIN32
        int pageSize = sysconf( _SC_PAGESIZE );

        off_t mapoff     = offset;
        off_t extraAtBeg = mapoff % pageSize;
        mapoff -= extraAtBeg;
        size += static_cast<size_t>( extraAtBeg );
        // map shared so the os can re-use, but it's read only anyway?
        void *p = mmap( nullptr, size, PROT_READ, MAP_SHARED, fd, mapoff );
        if ( p == MAP_FAILED )
            throw_errno(
                "Unable to map read only {0} bytes at offset {1} of file {2}",
                size,
                mapoff,
                fd );
#else
        SYSTEM_INFO si;
        GetSystemInfo( &si );
        off_t mapoff     = offset;
        off_t extraAtBeg = mapoff % si.dwPageSize;
        mapoff -= extraAtBeg;
        size += static_cast<size_t>( extraAtBeg );
        DWORD high = static_cast<DWORD>(
            uint64_t( ( uint64_t( mapoff ) >> 32 ) & 0xFFFFFFFF ) );
        DWORD low = mapoff & 0xFFFFFFFF;
        void *p   = MapViewOfFile( fd, FILE_MAP_ALL_ACCESS, high, low, size );
#endif
        char *tmp           = static_cast<char *>( p );
        _ptr                = reinterpret_cast<pointer>( tmp + extraAtBeg );
        _extra_at_beginning = extraAtBeg;
        _sz                 = size;
    }

    inline void swap( read_memory_map &mm )
    {
        std::swap( _ptr, mm._ptr );
        std::swap( _extra_at_beginning, mm._extra_at_beginning );
        std::swap( _sz, mm._sz );
    }

    inline pointer get( void ) const noexcept { return _ptr; }

    inline reference operator[]( size_t i ) const { return get()[i]; }

    inline pointer operator->(void)const noexcept { return get(); }

    explicit inline operator bool( void ) const noexcept
    {
        return get() == pointer() ? false : true;
    }

private:
    pointer _ptr = nullptr;
    // often better to besome O.S. need things to be page-aligned in certain scenarios
    // let's store that just in case...
    size_t _extra_at_beginning = 0;
    size_t _sz                 = 0;
};

////////////////////////////////////////

template <typename T, typename U>
inline bool
operator==( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    return x.get() == y.get();
}

template <typename T, typename U>
inline bool
operator!=( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    return x.get() != y.get();
}

template <typename T, typename U>
inline bool
operator<( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    using common = typename std::common_type<
        typename read_memory_map<T>::pointer,
        typename read_memory_map<U>::pointer>::type;
    return std::less<common>( x.get(), y.get() );
}

template <typename T, typename U>
inline bool
operator<=( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    return !( y < x );
}

template <typename T, typename U>
inline bool
operator>( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    return ( y < x );
}

template <typename T, typename U>
inline bool
operator>=( const read_memory_map<T> &x, const read_memory_map<U> &y ) noexcept
{
    return !( x < y );
}

////////////////////////////////////////

template <typename T>
inline bool operator==( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return !x;
}

template <typename T>
inline bool operator==( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return !x;
}

template <typename T>
inline bool operator!=( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return static_cast<bool>( x );
}

template <typename T>
inline bool operator!=( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return static_cast<bool>( x );
}

template <typename T>
inline bool operator<( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return std::less<typename read_memory_map<T>::pointer>( x.get(), nullptr );
}

template <typename T>
inline bool operator<( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return std::less<typename read_memory_map<T>::pointer>( nullptr, x.get() );
}

template <typename T>
inline bool operator<=( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return !( nullptr < x );
}

template <typename T>
inline bool operator<=( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return !( x < nullptr );
}

template <typename T>
inline bool operator>( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return std::less<typename read_memory_map<T>::pointer>( nullptr, x.get() );
}

template <typename T>
inline bool operator>( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return std::less<typename read_memory_map<T>::pointer>( x.get(), nullptr );
}

template <typename T>
inline bool operator>=( const read_memory_map<T> &x, std::nullptr_t ) noexcept
{
    return !( x < nullptr );
}

template <typename T>
inline bool operator>=( std::nullptr_t, const read_memory_map<T> &x ) noexcept
{
    return !( nullptr < x );
}

} // namespace base

namespace std
{
/// @brief provide std::swap overload for read_memory_map
template <typename T>
inline void swap( base::read_memory_map<T> &a, base::read_memory_map<T> &b )
{
    a.swap( b );
}

/// todo: do we need std::hash?

} // namespace std
