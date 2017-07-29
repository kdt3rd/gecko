#version 330

in vec2 tex_pos;
out vec4 frag_color;

uniform vec4 color;
uniform sampler2D textTex;

void main()
{
	vec4 texColor = texture( textTex, tex_pos );
	frag_color = vec4( color.r, color.g, color.b, color.a * texColor.r );
}

