
#pragma once

#include "opengl.h"
#include <string>

namespace gl
{

class program;
class context;

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
//		GEOMETRY = GL_GEOMETRY_SHADER
	};

	/// @brief Copying not allowed
	shader( const shader &s ) = delete;

	/// @brief Destructor
	~shader( void );

	/// @brief Set the source code
	void source( const std::string &code );

	/// @brief Compile the shader
	void compile( void );

	/// @brief Return the log messages
	std::string log( void );

	/// @brief Construct shader
	shader( type t );

	/// @brief Construct shader with code
	shader( type t, const std::string &code );

private:
	friend class program;
	friend class context;

	GLuint _shader;
};

////////////////////////////////////////

}
