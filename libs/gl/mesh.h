
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

/// @brief Mesh composed of vertex attributes and a program.
class mesh
{
public:
	mesh( void );

	~mesh( void );

	template<typename ...Args>
	void set_program( Args ...args )
	{
		_prog = std::make_shared<gl::program>( std::forward<Args>( args )... );
	}

	void add_triangles( size_t count, size_t start_vertex = 0 );
	void add_triangle_strip( size_t count, size_t start_vertex = 0 );
	void add_triangle_fan( size_t count, size_t start_vertex = 0 );
	void add_lines( size_t count, size_t start_vertex = 0 );
	void add_line_strip( size_t count, size_t start_vertex = 0 );
	void add_line_loop( size_t count, size_t start_vertex = 0 );

	void add_elements( primitive p, index_buffer_data &data );

	std::string log( void )
	{
		precondition( _prog, "program not created" );
		return _prog->log();
	}

	/// @brief Get attribute location
	program::attribute get_attribute_location( const std::string &name )
	{
		precondition( _prog, "program not created" );
		return _prog->get_attribute_location( name );
	}

	template<typename ...Args>
	void vertex_attribute( program::attribute loc, vertex_buffer_data<Args...> &data, size_t a = 0 )
	{
		precondition( _vao, "vertex array not created" );
		_vao->bind().attrib_pointer<Args...>( loc, data, a );
	}

	template<typename ...Args>
	void vertex_attribute( const std::string &name, vertex_buffer_data<Args...> &data, size_t a = 0 )
	{
		precondition( _vao, "vertex array not created" );
		precondition( _prog, "program not created" );
		_vao->bind().attrib_pointer<Args...>( _prog->get_attribute_location( name ), data, a );
	}

	void begin_draw( void )
	{
		precondition( _prog, "program not created" );
		_prog->use();
	}

	void draw( primitive prim, size_t start, size_t count )
	{
		precondition( _prog, "program not created" );
		precondition( _vao, "vertex array not created" );
		_vao->bind().draw_arrays( prim, start, count );
	}

	void draw( void );

	/// @brief Get uniform location
	program::uniform get_uniform_location( const std::string &name )
	{
		precondition( _prog, "program not created" );
		return _prog->get_uniform_location( name );
	}

	template<typename ...Args>
	void set_uniform( const std::string &name, Args ...args )
	{
		precondition( _prog, "program not created" );
		_prog->set_uniform( name, std::forward<Args>( args )... );
	}

	template<typename ...Args>
	void set_uniform( program::uniform u, Args ...args )
	{
		precondition( _prog, "program not created" );
		_prog->set_uniform( u, std::forward<Args>( args )... );
	}

	const std::shared_ptr<gl::program> &program( void ) const
	{
		return _prog;
	}

	void end_draw( void )
	{
		// TODO
	}

private:
	typedef std::tuple<primitive,size_t,size_t> Primitive;
	std::shared_ptr<vertex_array> _vao;
	std::shared_ptr<gl::program> _prog;
	std::vector<Primitive> _prims;
};

////////////////////////////////////////

}

