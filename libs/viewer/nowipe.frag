#version 330

vec4 wipe( vec4 a, vec4 b )
{
	return mix( b, a, a.a );
}

