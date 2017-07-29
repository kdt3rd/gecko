#version 330

layout(location = 0) in vec2 position;

uniform mat4 matrix;

void main()
{
	gl_Position.xy = position;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = matrix * gl_Position;
}

