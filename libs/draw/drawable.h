
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
	virtual ~drawable( void ) {}
	virtual void draw( gl::context &ctxt ) = 0;
};

////////////////////////////////////////

}

