#version 330

in vec2 uv;
out vec4 out_color;

uniform int num_chans;
uniform sampler2DRect tex_unitR;
uniform sampler2DRect tex_unitG;
uniform sampler2DRect tex_unitB;
uniform sampler2DRect tex_unitA;

void main()
{
    if ( num_chans == 1 )
    {
        out_color = texture( tex_unitR, uv );
        out_color.g = out_color.r;
        out_color.b = out_color.r;
        out_color.a = 1.0;
    }
    else if ( num_chans == 2 )
    {
        out_color = texture( tex_unitR, uv );
        out_color.g = texture( tex_unitG, uv ).g;
        out_color.b = 0.0;
        out_color.a = 1.0;
    }
    else if ( num_chans == 3 )
    {
        out_color = texture( tex_unitR, uv );
        out_color.g = texture( tex_unitG, uv ).g;
        out_color.b = texture( tex_unitB, uv ).b;
        out_color.a = 1.0;
    }
    else if ( num_chans == 4 )
    {
        out_color.r = texture( tex_unitR, uv ).r;
        out_color.g = texture( tex_unitG, uv ).g;
        out_color.b = texture( tex_unitB, uv ).b;
        out_color.a = texture( tex_unitA, uv ).a;
    }
}

