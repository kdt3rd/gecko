#version 330

in vec2 uv;
out vec4 out_color;

uniform sampler2DRect tex_unit;

void main()
{
    out_color = texture( tex_unit, uv );
}

