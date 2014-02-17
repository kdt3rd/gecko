#version 150

in vec2 text_out_coords;
in vec2 text_tex_coords;

uniform mat4 mvp_matrix;

out vec2 textCoord;

void main()
{
	textCoord = text_tex_coords;
	gl_Position = vec4( text_out_coords, 0.0, 1.0 );
	gl_Position = mvp_matrix * gl_Position;
}

