
#pragma once

#include "opengl.h"
#include "enums.h"
#include "vector.h"
#include <vector>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL index buffer object (IBO).
/// A IBO holds indices for drawing.
/// @sa gl::mesh
class index_buffer
{
public:
	/// @brief OpenGL bound index buffer object
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
		void data( const uint32_t *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );

		/// @brief Set the data in the buffer.
		void data( const std::vector<uint32_t> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

		/// @brief Change part of the data in the buffer.
		void sub_data( const uint32_t *data, size_t offset, size_t n );

		/// @brief Change part of the data in the buffer.
		void sub_data( const std::vector<uint32_t> &data, size_t offset );

		/// @brief Draw the primitive.
		void draw( primitive prim, size_t n );

		/// @brief Move operator.
		void operator=( binding &&other );

	private:
		friend class index_buffer;
		binding( GLuint buf );

		static binding *_bound;
	};

	/// @brief No copy constructor.
	index_buffer( const index_buffer &b ) = delete;

	/// @brief Default constructor.
	index_buffer( void );

	/// @brief Construct with data.
	index_buffer( const uint32_t *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW );

	/// @brief Construct with data.
	index_buffer( const std::vector<uint32_t> &data, buffer_usage u = buffer_usage::STATIC_DRAW );

	/// @brief Destructor.
	~index_buffer( void );

	/// @brief Bind the index buffer.
	binding bind( void );

	GLuint id( void ) const
	{
		return _buffer;
	}

private:
	GLuint _buffer;
};

////////////////////////////////////////

}

