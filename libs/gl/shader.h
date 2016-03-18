
#pragma once

#include "opengl.h"
#include <string>

namespace gl
{

class program;
class api;

////////////////////////////////////////

/// @brief OpenGL shader
class shader
{
public:
	/// @brief Types of shader
	enum class type
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER
	};

	/// @brief Copying not allowed
	shader( const shader &s ) = delete;

	/// @brief Construct shader
	shader( type t );

	/// @brief Construct shader with code
	shader( type t, const std::string &code );

	/// @brief Destructor
	~shader( void );

	/// @brief Set the source code
	void source( const std::string &code );

	/// @brief Compile the shader
	void compile( void );

	/// @brief Return the log messages
	std::string log( void );

private:
	friend class program;
	friend class api;

	GLuint _shader;
};

////////////////////////////////////////

}

