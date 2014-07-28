#version 330

uniform sampler2DRect txt;

in vec2 uv;

out vec4 out_color;

void main()
{
	out_color = texture( txt, uv );
}

