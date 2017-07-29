#version 330

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec2 char_pos;

uniform mat4 matrix;
out vec2 tex_pos;

void main()
{
	tex_pos = char_pos;
	gl_Position = matrix * vec4( vertex_pos, 0.0, 1.0 );
}

