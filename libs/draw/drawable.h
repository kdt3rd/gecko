
#pragma once

#include <memory>
#include <gl/vertex_array.h>

namespace gl
{
	class context;
}

namespace draw
{

////////////////////////////////////////

/// @brief Abstract drawable class.
class drawable
{
public:
	virtual ~drawable( void );
	drawable( void ) = default;
	drawable( const drawable & ) = default;
	drawable( drawable && ) = default;
	drawable &operator=( const drawable & ) = delete;
	drawable &operator=( drawable && ) = delete;

	virtual void draw( gl::api &ogl ) = 0;
};

////////////////////////////////////////

}

