#version 330

layout(location = 0) in vec2 position;

uniform mat4 matrix;
uniform vec2 top_left;

out vec2 uv;

void main()
{
	uv = position;
	gl_Position.xy = position + top_left;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = matrix * gl_Position;
}

