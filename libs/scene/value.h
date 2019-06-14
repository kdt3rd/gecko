// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "flags.h"

#include <base/const_string.h>
#include <iosfwd>
#include <memory>

////////////////////////////////////////

namespace scene
{
// hrm, see the types in media/metadata.h
enum class type_id
{
    v_reference, ///< reference to another value or shader output
    v_list,      ///< list of values

    v_string,
    v_filename,

    v_bool,

    v_bitmask8,
    v_bitmask16,
    v_bitmask32,
    v_bitmask64,

    v_int8,
    v_int16,
    v_int32,
    v_int64,
    v_uint8,
    v_uint16,
    v_uint32,
    v_uint64,

    v_float32,
    v_float64,

    v_colorf,
    v_colord,

    v_vec2i,
    v_vec3i,
    v_vec4i,
    v_vec2f,
    v_vec3f,
    v_vec4f,
    v_vec2d,
    v_vec3d,
    v_vec4d,

    v_box2i,
    v_box2f,
    v_box2d,
    v_box3i,
    v_box3f,
    v_box3d,

    v_mat22f,
    v_mat33f,
    v_mat44f,
    v_mat22d,
    v_mat33d,
    v_mat44d,

    // non-square matrices some things use
    v_mat2x3f,
    v_mat3x2f,
    v_mat2x4f,
    v_mat4x2f,
    v_mat3x4f,
    v_mat4x3f,
    v_mat2x3d,
    v_mat3x2d,
    v_mat2x4d,
    v_mat4x2d,
    v_mat3x4d,
    v_mat4x3d,

    v_int8_array,
    v_uint8_array,
    v_int16_array,
    v_uint16_array,
    v_int32_array,
    v_uint32_array,
    v_int64_array,
    v_uint64_array,
    v_float_array,
    v_double_array,

    v_colorf_array,
    v_colord_array,

    v_spectrum_f, ///< has an integer and range information at the front
    v_spectrum_d,

    v_vec2i_array,
    v_vec3i_array,
    v_vec4i_array,
    v_vec2f_array,
    v_vec3f_array,
    v_vec4f_array,
    v_vec2d_array,
    v_vec3d_array,
    v_vec4d_array,

    v_custom ///< plugin type
};

base::cstring type_name( type_id t );
type_id       type_from_name( base::cstring s );
size_t        type_sizeof( type_id t );

std::istream &operator>>( std::istream &i, type_id &t );
std::ostream &operator<<( std::ostream &o, type_id &t );

namespace detail
{
enum class value_flags : uint8_t
{
    at_default = 0,
    valid,
    animated,
    has_expression,
};

class interp
{
public:
    using calc_type = double;

    interp( void ) = default;
    virtual ~interp( void );

    virtual calc_type
    offset( const time &start, const time &end, const time &t ) const = 0;
};

template <typename T> struct value_segment
{
    using value_type = T;

    /// interpolates between this value and the next provided
    value_type interpolate( const time &t, const value_segment &next ) const
    {
        // TBD: for arrays, this is ... inefficient
        // TBD: for custom plugin types, this is ... impossible
        value_type delta = next._value - _value;
        return _value + delta * _interp->offset( _start, next._start, t );
    }

    time                    _start;
    std::unique_ptr<interp> _interp;

    value_type _value;
};

template <typename T> struct value_storage
{
    using segment    = value_segment<T>;
    using value_type = typename segment::value_type;

    std::vector<segment> _segments;
};

// TBD: move from vector to centralized storage pools
template <typename T> using value_array_storage = std::vector<T>;

template <typename T> class rle_array_storage
{
public:
    using value_type = T;

    inline size_t size( void ) const { return _size; }

    inline const value_type &operator[]( size_t idx ) const
    {
        auto   offset = _segments.begin();
        size_t sum    = 0;
        auto   ri     = _runs.begin();
        while ( ( sum + static_cast<size_t>( *ri ) ) < idx )
        {
            sum += *ri++;
            ++offset;
        }
        return ( *offset );
    }

    value_array_storage expand( void ) const
    {
        value_array_storage ret;
        ret.reserve( _size );
        auto ri = _runs.begin();
        auto vi = _segments.begin();
        while ( ri != _runs.end() )
            ret.insert( ret.end(), static_cast<size_t>( *ri++ ), *vi++ );
        return ret;
    }

    static rle_array_storage compact( const value_array_storage &v )
    {
        rle_array_storage ret;
        if ( v.empty() )
            return ret;

        ret._size             = v.size();
        auto       i          = v.begin();
        value_type last_value = ( *i );
        uint32_t   count      = 1;
        ++i;
        while ( i != v.end() )
        {
            if ( ( *i ) != last_value )
            {
                ret._segments.push_back( last_value );
                ret._runs.push_back( count );
                last_value = ( *i );
                count      = 1;
            }
            else
                ++count;
            ++i;
        }
        ret._segments.push_back( last_value );
        ret._runs.push_back( count );
    }

private:
    // TBD: move from vector to centralized storage pools
    std::vector<value_type> _segments;
    std::vector<uint32_t>   _runs;
    size_t                  _size = 0;
};

template <typename T, type_id TID> struct value_storage
{
public:
    static_assert(
        std::is_default_constructible<T>::value,
        "expect value type to be default constructible" );
    static_assert(
        std::is_copy_constructible<T>::value,
        "expect value type to be copy constructible" );
    static_assert(
        std::is_move_constructible<T>::value,
        "expect value type to be move constructible" );
    using value_type          = T;
    using animated_value_type = typename anim_curve<value_type>;

    static constexpr type_id k_type = TID;

    value_storage( void ) = default;

    static inline constexpr type_id type( void ) const { return k_type; }

    inline constexpr bool is_default( void ) const
    {
        return _flags.is_set<value_flags::at_default>();
    }
    inline constexpr bool is_valid( void ) const
    {
        return _flags.is_set<value_flags::valid>();
    }
    inline constexpr bool is_animated( void ) const
    {
        return _flags.is_set<value_flags::animated>();
    }
    inline constexpr bool has_expression( void ) const
    {
        return _flags.is_set<value_flags::has_expression>();
    }

    inline value_type evaluate( const time &t )
    {
        return ( is_animated() ) ? _animation->evaluate( t ) : _value;
    }

    inline void assign( const value_type &v );
    inline void assign( const time &t, const value_type &v )
    {
        if ( is_animated() ) {}
    }
    inline void reset( const value_type &v );

private:
    flags<value_flags> _flags; // 8-bit
    union
    {
        value_type           _value;
        animated_value_type *_animation;
    };
};

} // namespace detail

///
/// @brief Class value provides...
///
template <typename Storage> class value
{
public:
    using storage_type              = Storage;
    using value_type                = typename storage_type::value_type;
    static constexpr type_id k_type = storage_type::k_type;

    value( void );
    ~value( void );
    // TBD:
    value( const value & ) = delete;
    value &operator=( const value & ) = delete;
    value( value && )                 = delete;
    value &operator=( value && ) = delete;

    inline constexpr type_id type( void ) const noexcept { return k_type; }
    bool                     is_animated( void ) const;
    std::vector<time>        keyframes( void ) const;

    /// returns the range of time that should be invalidated based on setting the
    /// value at time t
    std::pair<time, time>   set( const time &t, const value_type &v );
    template <typename T> T evaluate( const time &t ) const
    {
        return _storage->evaluate<T>( t );
    }

private:
};

} // namespace scene
