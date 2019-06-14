// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "opengl.h"

#include <base/half.h>
#include <type_traits>

namespace gl
{
////////////////////////////////////////

/// @brief OpenGL capability
/// @see gl::api::enable
/// @see gl::api::disable
/// @todo Complete missing capabilities
enum class capability
{
    /// Blend the computed fragment color values with the values in the color buffers.
    /// @see gl::api::blend_func.
    /// @see gl::blend_style
    BLEND = GL_BLEND,

    /// Cull polygons based on their winding in window coordinates.
    /// @see gl::api::cull_face
    /// @todo Write cull face
    CULL_FACE = GL_CULL_FACE,

    /// Do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled.
    /// @see gl::api::depth_func
    /// @see gl::api::depth_range
    /// @todo Write depth_func and depth_range
    DEPTH_TEST = GL_DEPTH_TEST,

    /// Dither color components or indices before they are written to the color buffer.
    DITHER = GL_DITHER,

    /// If the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed.
    /// @see gl::api::polygon_offset
    /// @todo Write polygon_offset
    POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,

    /// Compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location. The temporary coverage value is then ANDed with the fragment coverage value.
    SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,

    /// Discard fragments that are outside the scissor rectangle.
    SCISSOR_TEST = GL_SCISSOR_TEST,

    /// Do stencil testing and update the stencil buffer.
    STENCIL_TEST = GL_STENCIL_TEST,

    /// Use multiple fragment samples in computing the final color of a pixel.
    MULTISAMPLE = GL_MULTISAMPLE,
};

////////////////////////////////////////

/// @brief OpenGL depth test
/// @see gl::api::depth_func
enum class depth_test
{
    /// Never passes.
    NEVER = GL_NEVER,

    /// Passes if the incoming depth value is less than the stored depth value.
    LESS = GL_LESS,

    /// Passes if the incoming depth value is equal to the stored depth value.
    EQUAL = GL_EQUAL,

    /// Passes if the incoming depth value is less than or equal to the stored depth value.
    LEQUAL = GL_LEQUAL,

    /// Passes if the incoming depth value is greater than the stored depth value.
    GREATER = GL_GREATER,

    /// Passes if the incoming depth value is not equal to the stored depth value.
    NOTEQUAL = GL_NOTEQUAL,

    /// Passes if the incoming depth value is greater than or equal to the stored depth value.
    GEQUAL = GL_GEQUAL,

    /// Always passes.
    ALWAYS = GL_ALWAYS
};

////////////////////////////////////////

/// @brief OpenGL blending styles
enum class blend_style
{
    ZERO                = GL_ZERO,
    ONE                 = GL_ONE,
    SRC_ALPHA           = GL_SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
};

////////////////////////////////////////

enum class buffer_bit
{
    COLOR_BUFFER_BIT   = GL_COLOR_BUFFER_BIT,
    DEPTH_BUFFER_BIT   = GL_DEPTH_BUFFER_BIT,
    STENCIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT,
};

typedef std::underlying_type<buffer_bit>::type buffer_bits;

inline buffer_bits operator|( buffer_bit a, buffer_bit b )
{
    return static_cast<buffer_bits>( a ) | static_cast<buffer_bits>( b );
}

inline buffer_bits operator|( buffer_bits a, buffer_bit b )
{
    return a | static_cast<buffer_bits>( b );
}

inline buffer_bits operator|( buffer_bit a, buffer_bits b )
{
    return static_cast<buffer_bits>( a ) | b;
}

////////////////////////////////////////

enum class data_type
{
    BYTE           = GL_BYTE,
    UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
    SHORT          = GL_SHORT,
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    INT            = GL_INT,
    UNSIGNED_INT   = GL_UNSIGNED_INT,
    FLOAT          = GL_FLOAT,
    DOUBLE         = GL_DOUBLE,
};

/// @brief OpenGL data type based on templating.
/// @tparam T Data type to get OpenGL enum for.
template <typename T> class gl_data_type
{
public:
    enum
    {
        /// OpenGL data type.
        value
    };
};

/// @cond
template <> class gl_data_type<uint8_t>
{
public:
    enum
    {
        value = GL_UNSIGNED_BYTE
    };
};

template <> class gl_data_type<int8_t>
{
public:
    enum
    {
        value = GL_BYTE
    };
};

template <> class gl_data_type<uint16_t>
{
public:
    enum
    {
        value = GL_UNSIGNED_SHORT
    };
};

template <> class gl_data_type<int16_t>
{
public:
    enum
    {
        value = GL_SHORT
    };
};

template <> class gl_data_type<uint32_t>
{
public:
    enum
    {
        value = GL_UNSIGNED_INT
    };
};

template <> class gl_data_type<int32_t>
{
public:
    enum
    {
        value = GL_INT
    };
};

template <> class gl_data_type<float>
{
public:
    enum
    {
        value = GL_FLOAT
    };
};

template <> class gl_data_type<double>
{
public:
    enum
    {
        value = GL_DOUBLE
    };
};

template <> class gl_data_type<base::half>
{
public:
    enum
    {
        value = GL_HALF_FLOAT
    };
};

/// @endcond

////////////////////////////////////////

enum class uniform_type
{
    FLOAT      = GL_FLOAT,
    FLOAT_VEC2 = GL_FLOAT_VEC2,
    FLOAT_VEC4 = GL_FLOAT_VEC4,
    FLOAT_MAT4 = GL_FLOAT_MAT4,
    OTHER      = 0,
};

////////////////////////////////////////

/// @brief Buffer usage
enum class buffer_usage
{
    STREAM_DRAW  = GL_STREAM_DRAW,
    STATIC_DRAW  = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
};

////////////////////////////////////////

enum class image_type
{
    UNSIGNED_BYTE          = GL_UNSIGNED_BYTE,
    UNSIGNED_SHORT_5_6_5   = GL_UNSIGNED_SHORT_5_6_5,
    UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
    UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
    // 10-bit
    UNSIGNED_INT_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
    // 16-bit
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    HALF           = GL_HALF_FLOAT,
    // 32-bit
    UNSIGNED_INT = GL_UNSIGNED_INT,
    FLOAT        = GL_FLOAT,
};

////////////////////////////////////////

enum class format
{
    RED  = GL_RED,
    RG   = GL_RG,
    RGB  = GL_RGB,
    RGBA = GL_RGBA,

    RED_HALF  = GL_R16F,
    RG_HALF   = GL_RG16F,
    RGB_HALF  = GL_RGB16F,
    RGBA_HALF = GL_RGBA16F,

    RED_FLOAT  = GL_R32F,
    RG_FLOAT   = GL_RG32F,
    RGB_FLOAT  = GL_RGB32F,
    RGBA_FLOAT = GL_RGBA32F,
};

////////////////////////////////////////

enum class swizzle
{
    RED   = GL_RED,
    GREEN = GL_GREEN,
    BLUE  = GL_BLUE,
    ALPHA = GL_ALPHA,
    ONE   = GL_ONE,
    ZERO  = GL_ZERO,
};

////////////////////////////////////////

enum class wrapping
{
    CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    REPEAT          = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
};

////////////////////////////////////////

enum class filter
{
    NEAREST                = GL_NEAREST,
    LINEAR                 = GL_LINEAR,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR,
};

////////////////////////////////////////

enum class primitive
{
    POINTS         = GL_POINTS,
    LINES          = GL_LINES,
    LINE_STRIP     = GL_LINE_STRIP,
    LINE_LOOP      = GL_LINE_LOOP,
    TRIANGLES      = GL_TRIANGLES,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    TRIANGLE_FAN   = GL_TRIANGLE_FAN,
};

////////////////////////////////////////

} // namespace gl
