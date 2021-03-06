#version 330

uniform sampler2DRect txt;
uniform vec2 center;
uniform float radius;

in vec2 uv;

out vec4 out_color;

void main()
{
	const float PI = 3.1415926535897932384626433832795;
	vec2 tmp = uv - center;
	float d = ( atan( -tmp.x, tmp.y ) / PI ) / 2.0 + 0.5;
	out_color = texture( txt, vec2( d*127 + 0.5, 0.5 ) );
}

