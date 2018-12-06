//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <memory>
#include <vector>
#include <array>
#include <base/meta.h>
#include "api.h"
#include "program.h"
#include "vertex_array.h"

namespace gl
{

////////////////////////////////////////

/// @brief Mesh composed of vertex attributes, elements and a program.
/// Mesh are created by setting up a program, setting up vertex attributes, and
/// adding elements.
class mesh
{
public:
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

		/// @brief Bind vertex data to use for an attribute.
		template<typename ...Args>
		void vertex_attribute( program::attribute loc, vertex_buffer_data<Args...> &data, size_t a = 0 )
		{
			_bound.attrib_pointer<Args...>( loc, data, a );
		}

		/// @brief Bind vertex data to use for an attribute.
		template<typename ...Args>
		void vertex_attribute( const std::string &name, vertex_buffer_data<Args...> &data, size_t a = 0 )
		{
			_bound.attrib_pointer<Args...>( _prog->get_attribute_location( name ), data, a );
		}

		/// @brief Bind elements data to draw from.
		void set_elements( element_buffer_data &data );

		/// @brief Set uniform value.
		template<typename ...Args>
		void set_uniform( const std::string &name, Args ...args )
		{
			precondition( _prog, "program not created" );
			_prog->set_uniform( name, std::forward<Args>( args )... );
		}

		/// @brief Set uniform value.
		template<typename ...Args>
		void set_uniform( program::uniform u, Args ...args )
		{
			precondition( _prog, "program not created" );
			_prog->set_uniform( u, std::forward<Args>( args )... );
		}

		/// @brief Draw all elements.
		void draw( void );

	private:
		friend class mesh;

		binding( mesh *m, vertex_array *vao, program *prog );

		bool _elements;
		vertex_array::binding _bound;
		mesh *_mesh;
		vertex_array *_vao;
		program *_prog;
	};

	/// @brief Default constructor.
	mesh( void );

	/// @brief Get the program for this mesh.
	gl::program &get_program( void );

	/// @brief Set the program for this mesh.
	void set_program( const std::shared_ptr<gl::program> &p )
	{
		precondition( p, "null program" );
		_prog = p;
	}

	template<typename ...Shaders>
	void set_program( const std::shared_ptr<shader> &s, Shaders ...shaders )
	{
		_prog = std::make_shared<program>( s, std::forward<Shaders>( shaders )... );
	}

	bool has_program( void )
	{
		return static_cast<bool>( _prog );
	}

	/// @brief Add points to draw.
	void add_points( size_t count, size_t start_vertex = 0 );

	/// @brief Add triangles to draw.
	void add_triangles( size_t count, size_t start_vertex = 0 );

	/// @brief Add a triangle strip to draw.
	void add_triangle_strip( size_t count, size_t start_vertex = 0 );

	/// @brief Add a triangle fan to draw.
	void add_triangle_fan( size_t count, size_t start_vertex = 0 );

	/// @brief Add lines to draw.
	void add_lines( size_t count, size_t start_vertex = 0 );

	/// @brief Add a line strip to draw.
	void add_line_strip( size_t count, size_t start_vertex = 0 );

	/// @brief Add a line loop to draw.
	void add_line_loop( size_t count, size_t start_vertex = 0 );

	/// @brief Get attribute location
	program::attribute get_attribute_location( const std::string &name )
	{
		precondition( _prog, "program not created" );
		return _prog->get_attribute_location( name );
	}

	/// @brief Get uniform location
	program::uniform get_uniform_location( const std::string &name )
	{
		precondition( _prog, "program not created" );
		return _prog->get_uniform_location( name );
	}

	/// @brief Set uniform value.
	template<typename ...Args>
	void set_uniform( const std::string &name, Args ...args )
	{
		precondition( _prog, "program not created" );
		_prog->set_uniform( name, std::forward<Args>( args )... );
	}

	/// @brief Set uniform value.
	template<typename ...Args>
	void set_uniform( program::uniform u, Args ...args )
	{
		precondition( _prog, "program not created" );
		_prog->set_uniform( u, std::forward<Args>( args )... );
	}

	/// @brief Bind the mesh.
	binding bind( void );

	void clear( void );
	void clear_program( void );

	bool valid( void ) const;

	const std::shared_ptr<vertex_array> &vertex_array_object( void ) const
	{
		return _vao;
	}

private:
	typedef std::tuple<primitive,size_t,size_t> Primitive;
	std::shared_ptr<vertex_array> _vao;
	std::shared_ptr<gl::program> _prog;
	std::vector<Primitive> _prims;
};

////////////////////////////////////////

}

