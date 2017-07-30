#version 330

layout(location = 0) in vec2 position;

uniform mat4 matrix;
uniform vec2 shape;
uniform vec2 resize;
uniform vec2 top_left;

out vec2 uv;

void main()
{
	uv = position;
	if ( uv.x < 0 )
		uv.x += shape.x;
	else
		uv.x += resize.x - shape.x;

	if ( uv.y < 0 )
		uv.y += shape.y;
	else
		uv.y += resize.y - shape.y;

	uv += top_left;

	gl_Position.xy = uv;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = matrix * gl_Position;
}

