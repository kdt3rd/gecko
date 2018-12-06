//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <base/variant.h>
#include <base/const_string.h>
//#include <experimental/any>
#include <base/half.h>
#include <base/rect.h>
#include <base/any.h>
#include <map>


////////////////////////////////////////


namespace media
{

using area_rect = base::rect<int64_t>;

///
/// Metadata types. We provide and understand some kinds of metadata by default
/// but other types need to be able to be registered by the metadata provider
/// and consumer
///
/// exif types:
/// val bytes type
/// 1   1     unsigned byte
/// 2   1     ascii string
/// 3   2     unsigned short
/// 4   4     unsigned long
/// 5   8     unsigned rational (2x long)
/// 6   1     signed byte
/// 7   1     undefined
/// 8   2     signed short
/// 9   4     signed long
/// 10  8     signed rational
/// 11  4     single float
/// 12  8     double float
///
/// any of the above can be an array (size in file is n elements, not bytes)
///
/// tiff type       bytes
/// byte            1
/// ascii           1
/// unsigned short  2
/// unsigned long   4
/// rational        8 (2xunsigned long)
/// signed byte     1
/// signed short    2
/// signed long     4
/// signed rational 8 (2x long)
/// float           4
/// double          8
/// ifd             4 (%32-bit unsigned integer)
/// long8           8
/// signed long8    8
/// ifd8            8 (%64-bit unsigned integer)
/// 
/// exr attributes
///   basic types   bytes
/// ---------------------
/// unsigned char   1
/// short           2
/// unsigned short  2
/// int             4
/// unsigned int    4
/// unsigned long   8
/// half            2
/// float           4
/// double          8
///
///   predefined attr types
/// -----------------------
///   box2i          4 int
///   box2f          4 float
///   chlist         sequence of channels followed by a null byte (0x00
///                  channel
///                    name       : 0-terminated string, from 1 - 255 bytes
///                    pixel_type : int (0 uint, 1 half, 2 float)
///                    pLinear    : unsigned char (0 or 1)
///                    reserved   : 3 char, should be 0
///                    xSampling  : int
///                    ySampling  : int
///   chromaticities 8 float (red.x, red.y, green.x, green.y, blue.x, blue.y, white.x, white.y)
///   compression    unsigned char (enum)
///   double         double
///   envmap         unsigned char (enum)
///   float          float
///   int            int
///   keycode        7 int : filmMfcCode, filmType, prefix, count, perfOffset, perfsPerFrame, perfsPerCnt
///   lineOrder      unsigned char (enum)
///   m33f           9 float
///   m44f           16 float
///   preview        2 unsigned int (w,h), followed by 4*w*h unsigned char
///   rational       int followed by unsigned int
///   string         int followed by sequence of char
///   stringvector   0 or more string
///   tiledesc       2 unsigned ints (x,y size) followed by mode (unsigned char)
///   timecode       2 unsigned int
///   v2i            2 int
///   v2f            2 float
///   v3i            3 int
///   v3f            3 float
///
///
///
///   both exif and tiff use a uint16 to denote the entry (which others have mapped to a string table)
///   exr has a string name for each metadata attribute
///
/// XMP has the following types:
/// property
///   - have qualifiers (another property)
/// structure (name -> property bag)
/// arrays
///   - unordered (bag)
///   - ordered (sequence)
///   - alternative (supposed to choose one of, i.e. multiple languages)
/// 
/// qualifiers (attached to simple property value, further describe simple property value)
///

////////////////////////////////////////

// TODO: redo this to be able stash off metadata...

using metadata_to_utf8_func = char *(*)( uint32_t meta_type, void *meta_value );
using metadata_from_utf8_func = void *(*)( uint32_t meta_type, const char *meta_string );

uint32_t register_metadata_type(
    const char *name,
    metadata_to_utf8_func serialize,
    metadata_from_utf8_func deserialize
                                );

/// @brief find a tag, may return uint32_t(-1) if not found
uint32_t find_metadata_type( const char *name );
/// @brief find a tag name, may return null
const char *metadata_type_name( uint32_t tag );

////////////////////////////////////////

struct metadata_value
{
    metadata_value( void ) = default;
    template <typename ... Args>
    metadata_value( uint32_t t, base::any v )
        : _type( t ), _storage( std::move( v ) )
    {}
    bool is_valid( void ) const { return _type != uint32_t(-1); }

    uint32_t type( void ) const { return _type; }

    template <typename U>
    inline void assign( uint32_t t, U &&v )
    {
        _type = t;
        _storage = std::forward<U>( v );
    }

    template <typename U>
    inline U as( void ) const
    {
        // ADL does not work because of the template specialization. rather than
        // put a blank any_cast in the global namespace, just use the particular
        // namespace...
        return base::any_cast<U>( _storage );
    }

    template <typename U>
    inline const U &as_ref( void ) const
    {
        // ADL does not work because of the template specialization. rather than
        // put a blank any_cast in the global namespace, just use the particular
        // namespace...
        return base::any_cast<const U &>( _storage );
    }

private:
    uint32_t _type = uint32_t(-1);
    // todo: if this changes to std::any, update as functions above
    base::any _storage;
};

////////////////////////////////////////

template <typename T>
class meta_size_and_value
{
public:
    using value_type = T;

    meta_size_and_value( size_t b ) : _size( b ) {}
    meta_size_and_value( const meta_size_and_value & ) = delete;
    meta_size_and_value( meta_size_and_value && ) = delete;
    meta_size_and_value &operator=( const meta_size_and_value & ) = delete;
    meta_size_and_value &operator=( meta_size_and_value && ) = delete;

    size_t size( void ) const
    {
        return _size;
    }

    const T *access( void ) const
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>( this );
        p += sizeof(*this);
        return reinterpret_cast<const T *>( p );
    }

    static meta_size_and_value *allocate( size_t bytes )
    {
        size_t bps = bytes + sizeof(size_t);
        void *p = ::malloc( bps );
        
        meta_size_and_value *sz = static_cast<meta_size_and_value *>( p );
        sz->size = bytes;
    }

    static void deallocate( void *ptr )
    {
        if ( ptr )
            ::free( ptr );
    }
private:
    size_t _size;
};

template <uint32_t tagval, typename T>
struct meta_value
{
    static const uint32_t tag = tagval;
    using value_type = T;
    static const size_t fixed_size = sizeof(value_type);

    static inline const value_type &retrieve( const metadata_value &v )
    {
        return v.as_ref<const value_type &>();
    }

    static inline metadata_value make( value_type v )
    {
        return metadata_value( tag, base::make_any<value_type>( std::move( v ) ) );
    }
};

template <uint32_t tagval>
struct meta_value<tagval, void>
{
    static const uint32_t tag = tagval;
    using value_type = void;
    static const size_t fixed_size = 0;

    static inline const void *retrieve( const metadata_value &v )
    {
        return v.as<const void *>();
    }

    static inline metadata_value make( value_type )
    {
        return metadata_value( tag, base::any() );
    }
};

///   
/// @brief metadata provides a container of metadata items.
///
/// The basic premise of this class is to be able to store metadata
/// such as random attributes from an OpenEXR file, or XMP data, or
/// similar. This data takes the form of being largely a string to
/// value map, where the value is of some set of types (including the
/// possiblity of having a metadata chunk itself to support having
/// structures).
///
/// NB: for now, let's just typedef metadata to a map to avoid
/// implementing anything until we see what we end up with
using metadata = std::map<std::string, metadata_value>;

/// TODO: make a formal type to ensure that the values all have the same type?
using metadata_list = std::vector<metadata_value>;

/// nameless enum since we're just using this to store the pre-defined types and
/// auto-increment an integer for us
enum
{
    meta_void_tag = 0,
    meta_group_tag, ///< a group of otherwise unconnected metadata values
    meta_list_tag, ///< a list of metadata values with same type

    meta_int8_tag,
    meta_uint8_tag,
    meta_int16_tag,
    meta_uint16_tag,
    meta_int32_tag,
    meta_uint32_tag,
    meta_int64_tag,
    meta_uint64_tag,

    meta_signed_rational32_tag,
    meta_unsigned_rational32_tag,
    meta_rational32_tag, ///< mixed types: int32_t / uint32_t

    meta_float16_tag,
    meta_float32_tag,
    meta_float64_tag,

    meta_ascii_string_tag,
    meta_ascii_stringlist_tag,
    meta_utf8_string_tag,
    meta_utf16_string_tag,
    meta_utf32_string_tag,

    meta_vec2_int32_tag,
    meta_vec2_float32_tag,
    meta_vec2_float64_tag,
    meta_vec3_int32_tag,
    meta_vec3_float32_tag,
    meta_vec3_float64_tag,

    meta_rect_int32_tag,
    meta_rect_float32_tag,

    meta_matrix33_float32_tag,
    meta_matrix33_float64_tag,
    meta_matrix44_float32_tag,
    meta_matrix44_float64_tag,

    meta_float32_list_tag,

    meta_chromaticities_tag,

    meta_keycode_tag,
    meta_timecode_tag,
};

using meta_void_t = meta_value<meta_void_tag, void>;
using meta_group_t = meta_value<meta_group_tag, metadata>;
using meta_list_t = meta_value<meta_list_tag, metadata_list>;

using meta_int8_t = meta_value<meta_int8_tag, int8_t>;
using meta_uint8_t = meta_value<meta_uint8_tag, uint8_t>;
using meta_int16_t = meta_value<meta_int16_tag, int16_t>;
using meta_uint16_t = meta_value<meta_uint16_tag, uint16_t>;
using meta_int32_t = meta_value<meta_int32_tag, int32_t>;
using meta_uint32_t = meta_value<meta_uint32_tag, uint32_t>;
using meta_int64_t = meta_value<meta_int64_tag, int64_t>;
using meta_uint64_t = meta_value<meta_uint64_tag, uint64_t>;

using meta_signed_rational32_t = meta_value<meta_signed_rational32_tag, std::pair<int32_t, int32_t> >;
using meta_unsigned_rational32_t = meta_value<meta_unsigned_rational32_tag, std::pair<uint32_t, uint32_t> >;
using meta_rational32_t = meta_value<meta_rational32_tag, std::pair<int32_t, uint32_t> >;

using meta_float16_t = meta_value<meta_float16_tag, base::half>;
using meta_float32_t = meta_value<meta_float32_tag, float>;
using meta_float64_t = meta_value<meta_float64_tag, double>;

using meta_string_t = meta_value<meta_ascii_string_tag, std::string>;
using meta_stringlist_t = meta_value<meta_ascii_stringlist_tag, std::vector<std::string> >;
using meta_utf8_string_t = meta_value<meta_utf8_string_tag, std::vector<uint8_t> >;
using meta_utf16_string_t = meta_value<meta_utf16_string_tag, std::vector<uint16_t> >;
using meta_utf32_string_t = meta_value<meta_utf32_string_tag, std::vector<uint32_t> >;

using meta_vec2i_t = meta_value<meta_vec2_int32_tag, std::array<int32_t, 2> >;
using meta_vec2f_t = meta_value<meta_vec2_float32_tag, std::array<float, 2> >;
using meta_vec2d_t = meta_value<meta_vec2_float64_tag, std::array<double, 2> >;
using meta_vec3i_t = meta_value<meta_vec3_int32_tag, std::array<int32_t, 3> >;
using meta_vec3f_t = meta_value<meta_vec3_float32_tag, std::array<float, 3> >;
using meta_vec3d_t = meta_value<meta_vec3_float64_tag, std::array<double, 3> >;

using meta_recti_t = meta_value<meta_rect_int32_tag, std::array<int32_t, 4> >;
using meta_rectf_t = meta_value<meta_rect_float32_tag, std::array<float, 4> >;

using meta_m33f_t = meta_value<meta_matrix33_float32_tag, std::array<float, 9> >;
using meta_m44f_t = meta_value<meta_matrix44_float32_tag, std::array<float, 16> >;
using meta_m33d_t = meta_value<meta_matrix33_float64_tag, std::array<double, 9> >;
using meta_m44d_t = meta_value<meta_matrix44_float64_tag, std::array<double, 16> >;

using meta_float_list_t = meta_value<meta_float32_list_tag, std::vector<float> >;

using meta_chromaticities_t = meta_value<meta_chromaticities_tag, std::array<float, 8> >;

using meta_keycode_t = meta_value<meta_keycode_tag, std::array<int32_t, 7> >;
using meta_smpte_timecode_t = meta_value<meta_timecode_tag, std::pair<uint32_t, uint32_t> >;

#define media_ImageDescription "img_desc"
#define media_ImageDisplayWin "disp_win"
#define media_ImagePixelAspectRatio "img_pixAR"

} // namespace media



