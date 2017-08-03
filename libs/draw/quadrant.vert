#version 330

layout(location = 0) in vec2 position;

uniform mat4 matrix;
uniform vec2 shape;
uniform vec2 resize;
uniform vec2 top_left;

out vec2 uv;

void main()
{
	vec2 tmp = position;
	if ( tmp.x < 0 )
		tmp.x += shape.x;
	else
		tmp.x += resize.x - shape.x;

	if ( tmp.y < 0 )
		tmp.y += shape.y;
	else
		tmp.y += resize.y - shape.y;

	tmp += top_left;

	gl_Position.xy = tmp;
	gl_Position.zw = vec2( 0.0, 1.0 );
	gl_Position = matrix * gl_Position;
	uv = position;
}

