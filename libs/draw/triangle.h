//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gl/mesh.h>
#include <gl/color.h>
#include <gl/matrix4.h>

namespace draw
{

////////////////////////////////////////

class triangle
{
public:
	triangle( void );

	void draw( gl::api &ogl, const gl::matrix4 &m );

private:
	void initialize( gl::api &ogl );
//	void resize( void );

	bool _init = false;
	gl::mesh _mesh;
	gl::program::uniform _matrix_loc;
};


////////////////////////////////////////

}

