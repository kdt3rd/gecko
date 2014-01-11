
#pragma once

#include "opengl.h"
#include <string>

namespace gl
{

class program;
class context;

////////////////////////////////////////

class shader
{
public:
	enum class type 
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER
	};

	shader( const shader &s ) = delete;

	~shader( void );

	void source( const std::string &code );
	void compile( void );

	std::string log( void );

	shader( type t );
	shader( type t, const std::string &code );

private:
	friend class program;
	friend class context;

	GLuint _shader;
};

////////////////////////////////////////

}

