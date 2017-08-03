#version 330

uniform sampler2DRect txt;
uniform vec2 center;
uniform float min_radius;
uniform float max_radius;

in vec2 uv;

out vec4 out_color;

void main()
{
	const float PI = 3.1415926535897932384626433832795;
	vec2 tmp = uv - center;
	float d = ( length( tmp ) - min_radius ) / ( max_radius - min_radius );
	out_color = texture( txt, vec2( d*128, 0.5 ) );
}

