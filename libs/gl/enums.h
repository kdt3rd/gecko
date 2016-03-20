
#pragma once

#include <type_traits>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL capability
///
/// @see gl::api::enable
/// @see gl::api::disable
/// @todo Complete missing capabilities
enum class capability
{
	/// If enabled, blend the computed fragment color values with the values in the color buffers.
	/// @see gl::api::blend_func.
	/// @see gl::blend_style
	BLEND = GL_BLEND,

	/// If enabled, cull polygons based on their winding in window coordinates.
	/// @see gl::api::cull_face
	/// @todo Write cull face
	CULL_FACE = GL_CULL_FACE,

	/// If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled.
	/// @see gl::api::depth_func
	/// @see gl::api::depth_range
	/// @todo Write depth_func and depth_range
	DEPTH_TEST = GL_DEPTH_TEST,

	/// If enabled, dither color components or indices before they are written to the color buffer.
	DITHER = GL_DITHER,

	/// If enabled, and if the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed.
	/// @see gl::api::polygon_offset
	/// @todo Write polygon_offset
	POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,

	SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,
	SCISSOR_TEST = GL_SCISSOR_TEST,
	STENCIL_TEST = GL_STENCIL_TEST,

};

////////////////////////////////////////

enum class depth_test
{
	NEVER = GL_NEVER,
	LESS = GL_LESS,
	EQUAL = GL_EQUAL,
	LEQUAL = GL_LEQUAL,
	GREATER = GL_GREATER,
	NOTEQUAL = GL_NOTEQUAL,
	GEQUAL = GL_GEQUAL,
	ALWAYS = GL_ALWAYS
};

////////////////////////////////////////

/// @brief OpenGL blending styles
enum class blend_style
{
	ZERO = GL_ZERO,
	ONE = GL_ONE,
	SRC_ALPHA = GL_SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
};

////////////////////////////////////////

enum class buffer_bit
{
	COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT,
	DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT,
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
	BYTE = GL_BYTE,
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	SHORT = GL_SHORT,
	UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	INT = GL_INT,
	UNSIGNED_INT = GL_UNSIGNED_INT,
	FLOAT = GL_FLOAT,
	DOUBLE = GL_DOUBLE,
};

template<typename D>
struct gl_data_type {};

template<>
struct gl_data_type<uint8_t>
{
	enum { value = GL_UNSIGNED_BYTE };
};

template<>
struct gl_data_type<int8_t>
{
	enum { value = GL_BYTE };
};

template<>
struct gl_data_type<uint16_t>
{
	enum { value = GL_UNSIGNED_SHORT };
};

template<>
struct gl_data_type<int16_t>
{
	enum { value = GL_SHORT };
};

template<>
struct gl_data_type<uint32_t>
{
	enum { value = GL_UNSIGNED_INT };
};

template<>
struct gl_data_type<int32_t>
{
	enum { value = GL_INT };
};

template<>
struct gl_data_type<float>
{
	enum { value = GL_FLOAT };
};

template<>
struct gl_data_type<double>
{
	enum { value = GL_DOUBLE };
};

////////////////////////////////////////

enum class uniform_type
{
	FLOAT = GL_FLOAT,
	FLOAT_VEC2 = GL_FLOAT_VEC2,
	FLOAT_VEC4 = GL_FLOAT_VEC4,
	FLOAT_MAT4 = GL_FLOAT_MAT4,
	OTHER = 0,
};

////////////////////////////////////////

/// @brief Buffer usage
enum class buffer_usage
{
	STREAM_DRAW = GL_STREAM_DRAW,
	STATIC_DRAW = GL_STATIC_DRAW,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
};

////////////////////////////////////////

enum class image_type
{
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
	UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
	UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
	HALF = GL_HALF_FLOAT,
	FLOAT = GL_FLOAT,
};

////////////////////////////////////////

enum class format
{
	RED = GL_RED,
	RG = GL_RG,
	RGB = GL_RGB,
	RGBA = GL_RGBA,

	RED_HALF = GL_R16F,
	RG_HALF = GL_RG16F,
	RGB_HALF = GL_RGB16F,
	RGBA_HALF = GL_RGBA16F,

	RED_FLOAT = GL_R32F,
	RG_FLOAT = GL_RG32F,
	RGB_FLOAT = GL_RGB32F,
	RGBA_FLOAT = GL_RGBA32F,
};

////////////////////////////////////////

enum class swizzle
{
	RED = GL_RED,
	GREEN = GL_GREEN,
	BLUE = GL_BLUE,
	ALPHA = GL_ALPHA,
	ONE = GL_ONE,
	ZERO = GL_ZERO,
};

////////////////////////////////////////

enum class wrapping
{
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	REPEAT = GL_REPEAT,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
};

////////////////////////////////////////

enum class filter
{
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

////////////////////////////////////////

enum class primitive
{
	POINTS = GL_POINTS,
	LINES = GL_LINES,
	LINE_STRIP = GL_LINE_STRIP,
	LINE_LOOP = GL_LINE_LOOP,
	TRIANGLES = GL_TRIANGLES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	TRIANGLE_FAN = GL_TRIANGLE_FAN,
};

////////////////////////////////////////

}

