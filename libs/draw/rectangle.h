
#pragma once

#include <gl/mesh.h>
#include <gl/color.h>
#include <gl/matrix4.h>

namespace draw
{

////////////////////////////////////////

class rectangle
{
public:
	rectangle( float x, float y, float w, float h, const gl::color &c = gl::white );

	void draw( gl::api &ogl, const gl::matrix4 &m );

	void resize( float x, float y, float w, float h );

private:
	void initialize( gl::api &ogl );

	float _x, _y, _w, _h;
	gl::color _color;
	bool _init = false;
	bool _resized = false;
	gl::mesh _mesh;
	gl::vertex_buffer_data<gl::vec2,gl::color> _vertices;
	gl::program::uniform _matrix_loc;
};


////////////////////////////////////////

}

