
#pragma once

#include <memory>
#include <gl/vertex_array.h>

namespace gl
{
	class context;
}

namespace gldraw
{

////////////////////////////////////////

class drawable
{
public:
	virtual ~drawable( void ) {}
	virtual void draw( gl::context &ctxt ) = 0;
};

////////////////////////////////////////

}

