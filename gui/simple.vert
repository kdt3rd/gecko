#version 440

layout(location = 0) in vec2 position;

uniform mat4 mvp_matrix;

void main()
{
	gl_Position.xy = position;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = mvp_matrix * gl_Position;
}

