#version 330

uniform sampler2DRect txt;
uniform vec2 point1;
uniform vec2 point2;
uniform float radius;

in vec2 uv;

out vec4 out_color;

void main()
{
	float dx = max( 0.0, max( min( point1.x, point2.x ) - uv.x, uv.x - max( point1.x, point2.x ) ) );
	float dy = max( 0.0, max( min( point1.y, point2.y ) - uv.y, uv.y - max( point1.y, point2.y ) ) );
	float d = length( vec2( dx, dy ) ) / radius;
	out_color = texture( txt, vec2( d*128, 0.5 ) );
}

