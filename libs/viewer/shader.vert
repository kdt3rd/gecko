#version 330

in vec2 position;
out vec2 uv;
uniform mat4 mvpMatrix;

void main()
{
	gl_Position = mvpMatrix * vec4( position, 0.0, 1.0 );
	uv = position;
}

