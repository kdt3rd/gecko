
#pragma once

#include "opengl.h"
#include "enums.h"
#include "vector.h"
#include <vector>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL buffer
class vertex_buffer
{
public:
	/// @brief OpenGL bound vertex buffer object
	class binding
	{
	public:
		binding( void ) = delete;
		binding( const binding &other ) = delete;
		binding( binding &&other );

		~binding( void );

		void data( const float *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );
		void data( const std::vector<float> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

		void sub_data( const float *data, size_t offset, size_t n );
		void sub_data( const std::vector<float> &data, size_t offset );

		void draw( primitive prim, size_t n );

		void operator=( binding &&other );

	private:
		friend class vertex_buffer;
		binding( GLuint buf );

		static binding *_bound;
	};

	vertex_buffer( const vertex_buffer &b ) = delete;

	vertex_buffer( void );
	vertex_buffer( const float *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );
	vertex_buffer( const std::vector<float> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

	~vertex_buffer( void );

	binding bind( void );

private:
	GLuint _buffer;
};

////////////////////////////////////////

}

