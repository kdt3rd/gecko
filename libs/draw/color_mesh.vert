#version 330

layout(location = 0) in vec2 position;

uniform mat4 matrix;
uniform vec4 color;

out vec4 v_color;

void main()
{
	gl_Position.xy = position;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = matrix * gl_Position;
	v_color = color;
}

