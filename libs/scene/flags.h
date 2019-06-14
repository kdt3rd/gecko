// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <atomic>
#include <type_traits>

////////////////////////////////////////

namespace scene
{
/// @brief Stores a (fixed) boolean flag set, changes are done thread
/// safely in a lock-free manner
///
/// This is primarily intended as a generic bitset for a small number
/// of bits (compared to std::bitset, which is for an arbitrary number
/// of bits). However, if the type provided is an enum, this assumes
/// the enum is contiguous starting at 0 and each enum is meant to be
/// accessed uniquely.
///
/// NB: There is currently a requirement that if an enum is used, it
/// be set and get 1 enum value at a time, since we are unable to
/// introspect into the enum. For the generic integer case, it is just
/// bit operations, so that is not an issue.
///
/// We are not using std::bitset as that seems to always allocate
/// values, and does not provide atomicity.
///
/// Thread Safety: This uses atomics, such that the flag can be set
/// without any sort of read / write lock. This is great for dirty
/// flags and other state bits, unless you need to trigger events when
/// they change. The bits in here are changed atomically, but with no
/// other ordering implied ("relaxed" ordering).
///
template <typename EnumOrIntType, size_t Bytes = sizeof( EnumOrIntType )>
class flags
{
public:
    static constexpr size_t storage_bytes = Bytes;

    /// Default construction is with no flags set
    constexpr flags( void ) noexcept : _storage( storage_type( 0 ) ) {}

    /// Initialize with a set of flags
    template <typename Val>
    explicit flags( Val initval ) : _storage( bit_location( initval ) )
    {}

    /// TBD: This should work to check multiple flags at the same time but only
    /// in the integer case, how to handle enums?
    template <typename Val> inline constexpr bool is_set( void ) const noexcept
    {
        return ( _storage.load( std::memory_order_relaxed ) &
                 bit_location<Val>() ) == bit_location<Val>();
    }

    template <typename Val> inline void set( void ) noexcept
    {
        _storage.fetch_or( bit_location<Val>(), std::memory_order_relaxed );
    }

    template <typename Val> inline void unset( void ) noexcept
    {
        _storage.fetch_and( ~bit_location<Val>(), std::memory_order_relaxed );
    }

    template <typename Val> inline void flip( void ) noexcept
    {
        _storage.fetch_xor( bit_location<Val>(), std::memory_order_relaxed );
    }

private:
    template <size_t N> storage{};
    template <> storage<1> { using type = uint8_t; };
    template <> storage<2> { using type = uint16_t; };
    template <> storage<4> { using type = uint32_t; };
    template <> storage<8> { using type = uint64_t; };

    using storage_type = storage<storage_bytes>::type;

    template <typename Val>
    static inline constexpr storage_type bit_location( Val v ) noexcept
    {
        return std::is_enum<Val>::value
                   ? static_cast<storage_type>( 1 << static_cast<int>( v ) )
                   : static_cast<storage_type>( v );
    }

    template <typename Val>
    static inline constexpr storage_type bit_location( void ) noexcept
    {
        static_assert(
            ( !std::is_enum<Val>::value ) ||
                ( size_t( Val ) < ( 8 * storage_bytes ) ),
            "invalid enum beyond storage size" );
        return std::is_enum<Val>::value
                   ? static_cast<storage_type>( 1 << static_cast<int>( Val ) )
                   : static_cast<storage_type>( Val );
    }

    std::atomic<storage_type> _storage;
};

} // namespace scene
