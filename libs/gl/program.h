
#pragma once

#include <memory>
#include <base/color.h>
#include <base/point.h>
#include <base/size.h>
#include "shader.h"
#include "matrix4.h"
#include "enums.h"

namespace gl
{

class context;

////////////////////////////////////////

/// @brief OpenGL program
class program
{
public:
	typedef GLuint uniform;
	typedef GLuint attribute;

	/// @brief Copying not allowed
	program( const program &program ) = delete;

	/// @brief Default constructor
	program( void );
	program( const std::shared_ptr<shader> &vertex );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment, const std::shared_ptr<shader> &geometry );

	/// @brief Constructor with shader(s)
	template<typename ...Shaders>
	program( const std::shared_ptr<shader> &s, Shaders ...shaders )
	{
		attach( s, std::forward<Shaders>( shaders )... );
		link();
	}

	/// @brief Destructor
	~program( void );

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

	/// @brief Set uniform array
	template <typename T>
	void set_uniform( const std::string &name, const T *values, size_t count )
	{
		set_uniform( get_uniform_location( name ), values, count );
	}

	/// @brief Set uniform integer
	void set_uniform( uniform uniform, int value );

	/// @brief Set uniform float
	void set_uniform( uniform uniform, float value );

	/// @brief Set uniform double
	void set_uniform( uniform uniform, double value );

	/// @brief Set uniform matrix
	void set_uniform( uniform uniform, const matrix4 &value );

	/// @brief Set uniform color
	void set_uniform( uniform uniform, const base::color &value );

	/// @brief Set uniform point
	void set_uniform( uniform uniform, const base::point &value );

	/// @brief Set uniform size
	void set_uniform( uniform uniform, const base::size &value );

	/// @brief Get number of uniforms
	size_t number_active_uniforms( void );

	/// @brief Get uniform information
	std::pair<uniform_type,std::string> active_uniform( size_t i );

private:
	friend class context;

	GLuint _program;
};

////////////////////////////////////////

}



