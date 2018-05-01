#version 330

in vec2 position;
out vec2 uv;
uniform mat4 matrix;

void main()
{
	gl_Position = matrix * vec4( position, 0.0, 1.0 );
	uv = position;
}

