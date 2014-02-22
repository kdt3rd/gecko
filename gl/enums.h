
#pragma once

#include <type_traits>

namespace gl
{

////////////////////////////////////////

enum class capability
{
	BLEND = GL_BLEND,
	CULL_FACE = GL_CULL_FACE,
	DEPTH_TEST = GL_DEPTH_TEST,
	DITHER = GL_DITHER,
	POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,
	SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,
	SCISSOR_TEST = GL_SCISSOR_TEST,
	STENCIL_TEST = GL_STENCIL_TEST,
};

////////////////////////////////////////

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
	FLOAT = GL_FLOAT,
};

template<typename D>
struct gl_data_type {};

template<>
struct gl_data_type<uint16_t>
{
	enum { value = GL_UNSIGNED_SHORT };
};

template<>
struct gl_data_type<uint8_t>
{
	enum { value = GL_UNSIGNED_BYTE };
};

template<>
struct gl_data_type<float>
{
	enum { value = GL_FLOAT };
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

enum class image_type
{
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
	UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
	UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
};

////////////////////////////////////////

enum class format
{
	RED = GL_RED,
	RG = GL_RG,
	RGB = GL_RGB,
	RGBA = GL_RGBA,
};

////////////////////////////////////////

enum class wrapping
{
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
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
	LINE_STRIP = GL_LINE_STRIP,
	LINE_LOOP = GL_LINE_LOOP,
	LINES = GL_LINES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	TRIANGLE_FAN = GL_TRIANGLE_FAN,
	TRIANGLES = GL_TRIANGLES,
	QUADS = GL_QUADS,
};

////////////////////////////////////////

}

