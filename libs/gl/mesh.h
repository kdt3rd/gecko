
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

	void draw( primitive prim, size_t start, size_t count )
	{
		precondition( _prog, "program not created" );
		precondition( _voa, "vertex array not created" );
		_prog->use();
		_voa->bind().draw( prim, start, count );
	}

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
	void vertex_attribute( program::attribute loc, vertex_buffer_data<Args...> &data, size_t a )
	{
		precondition( _voa, "vertex array not created" );
		_voa->bind().attrib_pointer<Args...>( loc, data, a );
	}

	void use( void )
	{
		precondition( _prog, "program not created" );
		_prog->use();
	}

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

private:
	std::shared_ptr<vertex_array> _voa;
	std::shared_ptr<gl::program> _prog;
};

////////////////////////////////////////

}

