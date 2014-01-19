#version 440

uniform sampler2DRect txt;
uniform vec2 origin;
uniform vec2 dir;

in vec2 uv;

out vec4 out_color;

void main()
{
	vec2 tmp = uv - origin;
	float l = length( dir );
	float d = dot( tmp, dir ) / ( l * l );
	out_color = texture( txt, vec2( d*128, 0.5 ) );
}

