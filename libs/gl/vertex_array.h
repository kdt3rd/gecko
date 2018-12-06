//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include "opengl.h"
#include "enums.h"
#include "program.h"
#include "vertex_buffer.h"
#include "element_buffer.h"
#include "vertex_buffer_data.h"
#include "element_buffer_data.h"
#include <vector>
#include <list>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL vertex array
class vertex_array
{
public:
	/// @brief OpenGL bound vertex array
	class binding
	{
	public:
		/// @brief No default constructor.
		binding( void ) = delete;

		/// @brief No copy constructor.
		binding( const binding & ) = delete;

		/// @brief Move constructor.
		binding( binding &&o );

		/// @brief Destructor
		~binding( void );

		/// @brief No copy operator.
		binding &operator=( const binding & ) = delete;

		/// @brief Move operator.
		binding &operator=( binding &&o );

		template<typename ...Args>
		void attrib_pointer( program::attribute attr, vertex_buffer_data<Args...> &data, size_t a )
		{
			precondition( a < data.attributes(), "invalid attribute" );
			auto vbo = data.vbo();
			attrib_pointer( attr, vbo, data.attribute_size( a ), data.stride(), data.attribute_offset( a ) );
		}

		void attrib_pointer( program::attribute attr, std::shared_ptr<vertex_buffer> &vbo, size_t components, size_t stride = 0, size_t offset = 0 );

		void bind_elements( element_buffer_data &data )
		{
			bind_elements( data.ebo() );
		}

		void bind_elements( const std::shared_ptr<element_buffer> &ebo );

		void draw_elements( primitive prim, size_t start, size_t count );
		void draw_arrays( primitive prim, size_t start, size_t count );

		template <typename T>
		void draw_elements( primitive prim, const std::vector<T> &buf )
		{
			precondition( !_self->has_element_buffer(), "can't draw from index vector without EBO" );
			glDrawElements( static_cast<GLenum>( prim ), buf.size(), gl_data_type<T>::value, buf.data() );
		}

	private:
		friend class vertex_array;
		binding( vertex_array *self);

		vertex_array *_self;
		static binding *_bound;
	};

	vertex_array( const vertex_array &b ) = delete;
	vertex_array( void );

	~vertex_array( void );

	binding bind( void );

	bool has_element_buffer( void ) const
	{
		return static_cast<bool>( _ebo );
	}

	GLuint id( void ) const
	{
		return _array;
	}

private:
	void add_vbo( const std::shared_ptr<vertex_buffer> &vbo )
	{
		_vbos.push_back( vbo );
	}

	friend class binding;
	GLuint _array;
	std::shared_ptr<element_buffer> _ebo;
	std::list<std::shared_ptr<vertex_buffer>> _vbos;
};

////////////////////////////////////////

}

