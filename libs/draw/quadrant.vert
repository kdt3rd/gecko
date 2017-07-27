#version 330

layout(location = 0) in vec2 position;

uniform mat4 mvp_matrix;
uniform vec2 center;
uniform vec2 top_left;
uniform vec2 quad_size;

out vec2 uv;

void main()
{
	uv = position;
	if ( uv.x > center.x )
		uv.x += quad_size.x - center.x*2;
	if ( uv.y > center.y )
		uv.y += quad_size.y - center.y*2;
	uv += top_left;
	gl_Position.xy = uv;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = mvp_matrix * gl_Position;
	uv = position;
}

