#version 150

in vec2 textCoord;

uniform vec4 color;
uniform sampler2D text_tex;

out vec4 out_color;

void main()
{
   vec4 c = texture( text_tex, textCoord );
   out_color = color * vec4( c.r, c.r, c.r, c.r );
}

