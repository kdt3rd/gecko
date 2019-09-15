// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <gl/color.h>
#include <gl/matrix4.h>
#include <gl/mesh.h>

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

    bool                 _init = false;
    gl::mesh             _mesh;
    gl::program::uniform _matrix_loc;
};

////////////////////////////////////////

} // namespace draw
