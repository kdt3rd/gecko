// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>

////////////////////////////////////////

namespace scene
{


/// Stores a boolean flag set. can be used with enums
///
/// if the type provided is an enum, this assumes the enum is
/// contiguous starting at 0 and each enum is meant to be accessed
/// uniquely.
///
/// Otherwise is just a generic bitset, and the values accessed can be
/// or'ed to check multiple flags at once.
///
/// NB: we are not using std::bitset as that seems to always 
template <typename EnumOrIntType, size_t Bytes = sizeof(EnumOrIntType)>
struct flags
{
    static constexpr bool is_enum = std::is_enum<EnumOrIntType>::value;
    static constexpr size_t storage_bytes = Bytes;

    /// This should work to check multiple flags at the same time
    template <typename Val>
    inline constexpr bool is_set( void ) const noexcept
    {
        return ( _storage & bit_location<Val>() ) == bit_location<Val>();
    }

    template <typename Val>
    void set( void ) noexcept
    {
        storage |= bit_location<Val>();
    }

    template <typename Val>
    void unset( void ) noexcept
    {
        _storage &= ~ bit_location<Val>();
    }

    template <typename Val>
    void flip( void ) noexcept
    {
        _storage ^= bit_location<Val>();
    }

private:
    template <size_t N> storage {};
    template <> storage<1> { using type = uint8_t; };
    template <> storage<2> { using type = uint16_t; };
    template <> storage<4> { using type = uint32_t; };
    template <> storage<8> { using type = uint64_t; };

    using storage_type = storage<storage_bytes>::type;

    template <typename Val>
    static inline constexpr storage_type bit_location( void ) noexcept
    {
        static_assert( (! is_enum) || (size_t(Val) < (8 * storage_bytes)), "invalid enum beyond storage size" );
        return is_enum ?
            static_cast<storage_type>( 1 << static_cast<int>( Val ) ) :
            static_cast<storage_type>( Val );
    }

    storage_type _storage;
};

} // namespace scene



