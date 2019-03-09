// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <utility>
#include "color.h"
#include "vector.h"
#include "shader.h"
#include "matrix4.h"
#include "enums.h"

namespace gl
{

class api;

////////////////////////////////////////

/// @brief OpenGL program
class program
{
public:
	typedef GLint uniform;
	typedef GLuint attribute;

	/// @brief Copying not allowed
	program( const program &program ) = delete;

	/// @brief Default constructor
	program( void );

	/// @brief Constructor with shader(s)
	template<typename ...Shaders>
	program( const std::shared_ptr<shader> &s, Shaders ...shaders )
		: program()
	{
		attach( s, std::forward<Shaders>( shaders )... );
		link();
	}

	/// @brief Destructor
	~program( void );

	/// @brief Attach and link shaders.
	template<typename ...Shaders>
	void set( Shaders ...shaders )
	{
		attach( shaders... );
		link();
	}

	/// @brief Attach shaders
	template<typename ...Shaders>
	void attach( const std::shared_ptr<shader> &s, Shaders ...shaders )
	{
		attach( s );
		attach( std::forward<Shaders>( shaders )... );
	}

	/// @brief Attach a shader
	void attach( const std::shared_ptr<shader> &s );

	/// @brief Link the program
	void link( void );

	/// @brief Use the program for rendering.
	void use( void );

	/// @brief Get log messages
	std::string log( void );

	/// @brief Get attribute location
	attribute get_attribute_location( const std::string &name );

	/// @brief Get uniform location
	uniform get_uniform_location( const std::string &name );

	/// @brief Set uniform value
	template <typename T>
	void set_uniform( const std::string &name, const T &value )
	{
		set_uniform( get_uniform_location( name ), value );
	}

	/// @brief Set uniform integer
	void set_uniform( uniform u, int value );

	/// @brief Set uniform float
	void set_uniform( uniform u, float value );

	/// @brief Set uniform double
	void set_uniform( uniform u, double value );

	/// @brief Set uniform matrix
	void set_uniform( uniform u, const matrix4 &value );

	/// @brief Set uniform color
	void set_uniform( uniform u, const color &value );

	/// @brief Set uniform vec4
	void set_uniform( uniform u, const vec4 &value );

	/// @brief Set uniform vec3
	void set_uniform( uniform u, const vec3 &value );

	/// @brief Set uniform vec2
	void set_uniform( uniform u, const vec2 &value );

	/// @brief Get number of uniforms
	size_t number_active_uniforms( void );

	/// @brief Get uniform information
	std::pair<uniform_type,std::string> active_uniform( size_t i );

private:
	GLuint _program;

	static program *_using;
};

////////////////////////////////////////

}



