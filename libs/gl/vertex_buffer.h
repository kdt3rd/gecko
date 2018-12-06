//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include "opengl.h"
#include "enums.h"
#include "vector.h"
#include <vector>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL vertex buffer object (VBO).
/// A VBO holds floating point data for vertices.
/// Multiple attributes can be stored in a VBO, or
/// multiple VBOs can be used with one attribute each.
/// @sa gl::vertex_array
class vertex_buffer
{
public:
	/// @brief OpenGL bound vertex buffer object
	class binding
	{
	public:
		/// @brief No default constructor.
		binding( void ) = delete;

		/// @brief No copy constructor.
		binding( const binding &other ) = delete;

		/// @brief Move constructor.
		binding( binding &&other );

		/// @brief Destructor.
		~binding( void );

		/// @brief Set the data in the buffer.
		void data( const float *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );

		/// @brief Set the data in the buffer.
		void data( const std::vector<float> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

		/// @brief Change part of the data in the buffer.
		void sub_data( const float *data, size_t offset, size_t n );

		/// @brief Change part of the data in the buffer.
		void sub_data( const std::vector<float> &data, size_t offset );

		/// @brief Draw the primitive.
		void draw( primitive prim, size_t n );

		/// @brief Move operator.
		void operator=( binding &&other );

	private:
		friend class vertex_buffer;
		binding( GLuint buf );

		static binding *_bound;
	};

	/// @brief No copy constructor.
	vertex_buffer( const vertex_buffer &b ) = delete;

	/// @brief Default constructor.
	vertex_buffer( void );

	/// @brief Construct with data.
	vertex_buffer( const float *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );

	/// @brief Construct with data.
	vertex_buffer( const std::vector<float> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

	/// @brief Destructor.
	~vertex_buffer( void );

	/// @brief Bind the vertex buffer.
	binding bind( void );

private:
	GLuint _buffer;
};

////////////////////////////////////////

}

