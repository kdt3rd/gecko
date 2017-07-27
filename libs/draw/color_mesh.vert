#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

uniform mat4 mvp_matrix;

out vec4 v_color;

void main()
{
	gl_Position.xy = position;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = mvp_matrix * gl_Position;
	v_color = color;
}

