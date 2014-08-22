#version 330

in vec2 uv;
out vec4 out_color;

vec4 sourceA();
vec4 sourceB();
vec4 lutA( vec4 a );
vec4 lutB( vec4 b );
vec4 wipe( vec4 a, vec4 b );

void main()
{
	vec4 a = sourceA();
	vec4 b = sourceB();

	a = lutA( a );
	b = lutB( b );

	vec4 r = wipe( a, b );

	vec4 bg = vec4( 0.2, 0.2, 0.2, 1.0 );
	int x = int( uv.x / 16 );
	int y = int( uv.y / 16 );
	if ( ( x + y ) % 2 == 0 )
		bg = vec4( 0.4, 0.4, 0.4, 1.0 );
	out_color = mix( bg, r, r.a );
}

